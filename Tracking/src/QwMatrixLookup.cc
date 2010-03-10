#include "QwMatrixLookup.h"

QwTrajMatrix* QwMatrixLookup::fMatrix = 0;

void QwMatrixLookup::SetStartAndEndPoints(TVector3 startposition, TVector3 startdirection,
                                        TVector3 endposition, TVector3 enddirection) {

    fStartPosition = startposition;
    fStartDirection = startdirection;
    fEndPosition = endposition;
    fEndDirection = enddirection;

    fStartPositionR = sqrt(fStartPosition.X()*fStartPosition.X()+fStartPosition.Y()*fStartPosition.Y());
    fStartPositionPhi = atan2(fStartPosition.Y(),fStartPosition.X())*DEGREE;
    if (fStartPositionPhi<0) fStartPositionPhi = fStartPositionPhi+360.0;

    fStartDirectionTheta = acos(fStartDirection.Z())*DEGREE;
    fStartDirectionPhi = atan2(fStartDirection.Y(),fStartDirection.X())*DEGREE;
    if (fStartDirectionPhi<0) fStartDirectionPhi = fStartDirectionPhi+360.0;

    fEndPositionR = sqrt(fEndPosition.X()*fEndPosition.X()+fEndPosition.Y()*fEndPosition.Y());
    fEndPositionPhi = atan2(fEndPosition.Y(),fEndPosition.X())*DEGREE;
    if (fEndPositionPhi<0) fEndPositionPhi = fEndPositionPhi+360.0;

    fEndDirectionTheta = acos(fEndDirection.Z())*DEGREE;
    fEndDirectionPhi = atan2(fEndDirection.Y(),fEndDirection.X())*DEGREE;
    if (fEndDirectionPhi<0) fEndDirectionPhi = fEndDirectionPhi+360.0;
};

int QwMatrixLookup::Bridge(QwPartialTrack* front, QwPartialTrack* back)
{
    // front track position and angles at z= -250 cm plane
    double x = front->GetPosition(-250.0).X();
    double y = front->GetPosition(-250.0).Y();

    double position_r = sqrt(x*x+y*y);
    double position_phi = fStartPositionPhi;
    double direction_theta = fStartDirectionTheta;

    double vertex_z = -250.0 - position_r/tan(acos(fStartDirection.Z()));

    // expected hit position and angles at z= +570 cm plane
    x = back->GetPosition(570.0).X();
    y = back->GetPosition(570.0).Y();

    double expectedhitposition_r = sqrt(x*x+y*y);
    double expectedhitposition_phi = atan2(y,x)*DEGREE;
    if (expectedhitposition_phi<0.0)
        expectedhitposition_phi = expectedhitposition_phi + 360.0;

    bool isintable =   ((int)position_r >= R_MIN) && ((int)position_r < R_MAX) &&
                       ((int)position_phi >= PHI_MIN) && ((int)position_phi <PHI_MAX) &&
                       ((int)vertex_z > VERTEXZ_MIN) && ((int)vertex_z < VERTEXZ_MAX);

    // search the table
    if ( ! isintable ) {
        std::cout<<"vertex_z="<<vertex_z<<std::endl;
        std::cout<<"position_r="<<position_r<<std::endl;
        std::cout<<"position_phi="<<position_phi<<std::endl;
        std::cout<<"direction_theta="<<direction_theta<<std::endl;
        std::cout<<"This potential track is not listed in the table."<<std::endl;
        return -1;
    }

    // find the index of the neighbour tracks
    int index_pos_r = ((int)(position_r+0.5)-R_MIN)/DR;
    int index_pos_phi = ((int)(position_phi+0.5)-PHI_MIN)/DPHI;
    int index_vertex_z = ((int)(vertex_z+0.5)-VERTEXZ_MIN)/DZ;

    // build two subsets of the table
    std::vector <QwPartialTrackParameter> backtrackparametersublist;
    QwPartialTrackParameter backtrackparameter;
    double *iP = new double[P_MAX-P_MIN+1];  //hold momentum for inteprolation
    double *iR = new double[P_MAX-P_MIN+1];  //hold radial position for inteprolation

    // NOTE jpan: ROOT::Math::GSLInterpolator::Eval(double) requires that
    // x values must be monotonically increasing.

    for (int p = P_MIN; p<=P_MAX; p+=DP) {
        // build index
        int index_momentum = ((int)p-P_MIN)/DP;

        unsigned int index = index_momentum*R_GRIDSIZE*PHI_GRIDSIZE*Z_GRIDSIZE
                             + index_pos_r*PHI_GRIDSIZE*Z_GRIDSIZE
                             + index_pos_phi*Z_GRIDSIZE
                             + index_vertex_z ;

        // keep iR monotonically increasing with the index
        int ind = (P_MAX-P_MIN)/DP-index_momentum;

        backtrackparameter = fMatrix->at(index);

        iP[ind] = ((double)p)/1000.0;
        iR[ind] = backtrackparameter.fPositionR;
        backtrackparametersublist.push_back( backtrackparameter );
        //std::cout<<"ind, iP, iR:   "<<ind<<", "<<iP[ind]<<", "<<iR[ind]<<std::endl;
    }

    //NOTE The dr/dp on focal plane is about 1 cm per 10 MeV

    if ( (backtrackparametersublist.front().fPositionR  < expectedhitposition_r) &&
            (backtrackparametersublist.back().fPositionR   > expectedhitposition_r) ) {
        std::cout<<"No match in look-up table!"<<std::endl;
        return -1;
    }

    // the hit is within the momentum limits, do interpolation for momentum

    // We can choose among the following methods:
    // CSPLINE, LINEAR, POLYNOMIAL,
    // CSPLINE_PERIODIC, AKIMA, AKIMA_PERIODIC
    UInt_t size = backtrackparametersublist.size();

#if defined __ROOT_HAS_MATHMORE && ROOT_VERSION_CODE >= ROOT_VERSION(5,18,0)

#if ROOT_VERSION_CODE >= ROOT_VERSION(5,22,0)
    // Newer version of the MathMore plugin use kPOLYNOMIAL
    ROOT::Math::Interpolator inter(size, ROOT::Math::Interpolation::kPOLYNOMIAL);
#else
    // Older version of the MathMore plugin use POLYNOMIAL
    ROOT::Math::Interpolator inter(size, ROOT::Math::Interpolation::POLYNOMIAL);
#endif

    inter.SetData(size, iR, iP);
    fMomentum = inter.Eval(expectedhitposition_r);  // + 0.0016;  // [GeV]

    delete iP;
    delete iR;

    // take the track parameter from the nearest track
    if (fMomentum<0.98 || fMomentum>1.165) {
        std::cerr<<"Out of momentum range: determined momentum by looking up table: "
        << fMomentum<<" GeV"<<std::endl;
        return -1;
    }

    backtrackparameter = backtrackparametersublist.at(((int)(fMomentum*1000.0+0.5)-P_MIN)/DP);

    // set matching info
    double sin_theta = sin(backtrackparameter.fDirectionTheta/DEGREE);
    double cos_theta = cos(backtrackparameter.fDirectionTheta/DEGREE);
    double sin_phi = sin(backtrackparameter.fDirectionPhi/DEGREE);
    double cos_phi = cos(backtrackparameter.fDirectionPhi/DEGREE);
    double sin_PHI = sin(backtrackparameter.fPositionPhi/DEGREE);
    double cos_PHI = cos(backtrackparameter.fPositionPhi/DEGREE);

    fHitLocation = TVector3(backtrackparameter.fPositionR*cos_PHI,
                            backtrackparameter.fPositionR*sin_PHI,
                            570.0);

    fHitDirection = TVector3(sin_theta*cos_phi,sin_theta*sin_phi,cos_theta);

    // extend to z = fEndPosition.Z() plane
    double r = fabs(fEndPosition.Z()-570.0)/fHitDirection.Z();
    x = fHitLocation.X() - r*fHitDirection.X();
    y = fHitLocation.Y() - r*fHitDirection.Y();
    fHitLocationR = sqrt(x*x+y*y);
    fHitLocationPhi = atan2(y,x)*DEGREE;
    if (fHitLocationPhi<0.0)  fHitLocationPhi += 360.0;
    fHitLocation = TVector3(x,y,fEndPosition.Z());

    fHitDirectionTheta = acos(fHitDirection.Z())*DEGREE;
    fHitDirectionPhi = atan2(fHitDirection.Y(),fHitDirection.X())*DEGREE;
    if (fHitDirectionPhi<0.0) fHitDirectionPhi+=360.0;

    // errors
    fPositionROff = fHitLocationR - fEndPositionR;
    fPositionPhiOff = fHitLocationPhi - fEndPositionPhi;
    fDirectionThetaOff = fHitDirectionTheta - fEndDirectionTheta;
    fDirectionPhiOff = fHitDirectionPhi - fEndDirectionPhi;

    fPositionXOff = fHitLocation.X() - fEndPosition.X();
    fPositionYOff = fHitLocation.Y() - fEndPosition.Y();

    fDirectionXOff = fHitDirection.X() - fEndDirection.X();
    fDirectionYOff = fHitDirection.Y() - fEndDirection.Y();
    fDirectionZOff = fHitDirection.Z() - fEndDirection.Z();

    if ( fabs(fPositionROff)<2.0 && fabs(fPositionPhiOff)<4.0 &&
            fabs(fDirectionThetaOff)<1.0 && fabs(fDirectionPhiOff)<4.0 ) {

        //std::cout<<"======>>>>> Found a good match in the look-up table"<<std::endl;
        fMatchFlag = 0;
        return 0;
    } else {
        std::cout<<"Didn't find a good match in the look-up table"<<std::endl;
        return -1;
    }

#else // defined __ROOT_HAS_MATHMORE && ROOT_VERSION_CODE >= ROOT_VERSION(5,18,0)

    // No support for ROOT MathMore: warn user and return failure
    std::cout << "No support for QwMatrixLookup interpolation." << std::endl;
    std::cout << "Please install the ROOT MathMore plugin and recompile." << std::endl;
    return -1;

#endif // defined __ROOT_HAS_MATHMORE && ROOT_VERSION_CODE >= ROOT_VERSION(5,18,0)

};


void QwMatrixLookup::GetBridgingResult(Double_t *buffer) {

    buffer[0] = fStartPosition.X();
    buffer[1] = fStartPosition.Y();
    buffer[2] = fStartPosition.Z();
    buffer[3] = fStartPositionR;
    buffer[4] = fStartPositionPhi;
    buffer[5] = fStartDirection.X();
    buffer[6] = fStartDirection.Y();
    buffer[7] = fStartDirection.Z();
    buffer[8] = fStartDirectionTheta;
    buffer[9] = fStartDirectionPhi;

    buffer[10] = fEndPosition.X();
    buffer[11] = fEndPosition.Y();
    buffer[12] = fEndPosition.Z();
    buffer[13] = fEndPositionR;
    buffer[14] = fEndPositionPhi;
    buffer[15] = fEndDirection.X();
    buffer[16] = fEndDirection.Y();
    buffer[17] = fEndDirection.Z();
    buffer[18] = fEndDirectionTheta;
    buffer[19] = fEndDirectionPhi;

    buffer[20] = fHitLocation.X();
    buffer[21] = fHitLocation.Y();
    buffer[22] = fHitLocation.Z();
    buffer[23] = fHitLocationR;
    buffer[24] = fHitLocationPhi;
    buffer[25] = fHitDirection.X();
    buffer[26] = fHitDirection.Y();
    buffer[27] = fHitDirection.Z();
    buffer[28] = fHitDirectionTheta;
    buffer[29] = fHitDirectionPhi;

    buffer[30] = fPositionROff;
    buffer[31] = fPositionPhiOff;
    buffer[32] = fDirectionThetaOff;
    buffer[33] = fDirectionPhiOff;

    // jpan: fMomentum is the determined momentum on the z=-250 cm plane.
    // In order to get the scattered momentum (P') inside the target,
    // we should use MC simulation to get the energy loss from scattering
    // vertex to z=-250 cm plane as a correction. This correction should
    // be added onto P' to get the final determined momentum.
    // When the correct scattering angle is determined, it could be used for
    // this purpose.

    double momentum_correction = 0.0; //0.032;  // assume 32 MeV with multi-scattering, etc.
    double PP = fMomentum + momentum_correction;
    buffer[34] = PP;

    double Mp = 0.938272013;    // Mass of the Proton
    double P0 = Mp*PP/(Mp-PP*(1-cos(fStartDirectionTheta/DEGREE))); //pre-scattering energy
    double Q2 = 2.0*Mp*(P0-PP);
    buffer[35] = P0;
    buffer[36] = Q2;

// NOTE Had to comment the fPrimary variables as they are not read by anymore
//     buffer[37] = fPrimary_OriginVertexKineticEnergy;
//     buffer[38] = fPrimary_PrimaryQ2;

//     double MomentumOff = PP-fPrimary_OriginVertexKineticEnergy;
//     double Q2_Off = Q2-fPrimary_PrimaryQ2;
//     buffer[39] = MomentumOff;
//     buffer[40] = Q2_Off;
//     buffer[41] = fPrimary_CrossSectionWeight;

    buffer[42] = fMatchFlag;
};


bool QwMatrixLookup::LoadTrajMatrix(const std::string filename)
{
  QwMatrixLookup::fMatrix = new QwTrajMatrix();
  QwMatrixLookup::fMatrix->ReadTrajMatrixFile(filename);
  return true;
};
