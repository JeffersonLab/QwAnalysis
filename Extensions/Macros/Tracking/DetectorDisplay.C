{
  gSystem->Load("libGeom");
  gSystem->Load("libEve");
  TString filename = Form(getenv("QWANALYSIS")) + TString("/Tracking/prminput/qweak.gdml");
  std::cout << filename << std::endl;
  TGeoManager* geomanager = TGeoManager::Import(filename);
  TEveManager* evemanager = TEveManager::Create();
  TGeoNode* topnode = geomanager->GetTopNode();
  TEveGeoTopNode* its = new TEveGeoTopNode(geomanager, topnode);
  evemanager->AddGlobalElement(its);
  evemanager->Redraw3D(kTRUE);
}
