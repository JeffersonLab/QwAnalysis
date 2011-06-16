#!/usr/bin/perl -w

# Script to generate DevTypes.hh
# R. Holmes Aug 2002
#
# Inputs: none
# Output to stdout.
#
# To add a new device of an existing type to DevTypes.hh, add its
# (unique) name to the appropriate list (or, for a new ADC or ADCX or VQWK or
# scaler, increase $adcnum or $adcxnum or $vqwknum of $scanum).
#
# To add a new type of device, add a list of device names and a sub
# do_<whatever> modelled on do_striplines, do_bcms, etc., and call it
# along with the rest of the do_<whatever> subroutines below.
#
# Note that in either case you must still make appropriate changes to
# e.g. TaDevice.cc in addition.

# Hall A stripline BPMs
@halla_strlist = qw / IBPM8 IBPM10 IBPM12 IBPM14 IBPM1 IBPM4A IBPM4B/;
# Injector stripline BPMs
@inj_strlist =   qw / IBPM1I02 IBPM1I04 IBPM1I06 IBPM0I01 IBPM0I01A IBPM0I02 IBPM0I02A IBPM0I05 IBPM0I07 IBPM0L01 IBPM0L02 IBPM0L03 IBPM0L04 IBPM0L05 IBPM0L06  IBPM0L07 IBPM0L08 IBPM0L09 IBPM0L10  IBPM0R03  IBPM0R04 IBPM0R05 IBPM0R06/;  
# Cavity BPMs
@cavlist =       qw / IBPMCAV1 IBPMCAV2 IBPMCAV3 IBPMCAV4 /;
# Old (HAPPEX-I era) current monitors
@old_bcmlist =   qw / IBCM1 IBCM2 IBCM3 IBCM4 IBCM5 IBCM6 IBCM7 IBCM8 IBCM9 IBCM10 IBCM11 IBCM12 IBCM13 /;
# G0 current monitors
@g0_bcmlist =    qw / IBCMCAV1 IBCMCAV2 IBCMCAV3 IBCMCAV4 /;

# DIS total shower on Left HRS
@dis_totsh_left = qw / IDISLTS1 IDISLTS2 IDISLTS3 IDISLTS4 IDISLTS5 IDISLTS6 /;
# DIS total shower on Right HRS
@dis_totsh_right = qw / IDISRTS1 IDISRTS2 IDISRTS3 IDISRTS4 IDISRTS5 IDISRTS6 IDISRTS7 IDISRTS8 /;
# DIS preshower on Left HRS
@dis_presh_left = qw / IDISLPS1 IDISLPS2 IDISLPS3 IDISLPS4 IDISLPS5 IDISLPS6 /;
# DIS preshower on Right HRS
@dis_presh_right = qw / IDISRPS1 IDISRPS2 IDISRPS3 IDISRPS4 IDISRPS5 IDISRPS6 IDISRPS7 IDISRPS8 /;
# DIS electron narrow on Left HRS
@dis_electnar_left = qw / IDISLEN1 IDISLEN2 IDISLEN3 IDISLEN4 IDISLEN5 IDISLEN6 /;
# DIS electron narrow on Right HRS
@dis_electnar_right = qw / IDISREN1 IDISREN2 IDISREN3 IDISREN4 IDISREN5 IDISREN6 IDISREN7 IDISREN8 /;
# DIS electron wide on Left HRS
@dis_electwid_left = qw / IDISLEW1 IDISLEW2 IDISLEW3 IDISLEW4 IDISLEW5 IDISLEW6 /;
# DIS electron wide on Right HRS
@dis_electwid_right = qw / IDISREW1 IDISREW2 IDISREW3 IDISREW4 IDISREW5 IDISREW6 IDISREW7 IDISREW8 /;
# DIS pion wide on Left HRS
@dis_pionwid_left = qw / IDISLPW1 IDISLPW2 IDISLPW3 IDISLPW4 IDISLPW5 IDISLPW6 /;
# DIS pion wide on Right HRS
@dis_pionwid_right = qw / IDISRPW1 IDISRPW2 IDISRPW3 IDISRPW4 IDISRPW5 IDISRPW6 IDISRPW7 IDISRPW8 /;
# DIS pion narrow on Left HRS
@dis_pionnar_left = qw / IDISLPN1 IDISLPN2 IDISLPN3 IDISLPN4 IDISLPN5 IDISLPN6 /;
# DIS pion narrow on Right HRS
@dis_pionnar_right = qw / IDISRPN1 IDISRPN2 IDISRPN3 IDISRPN4 IDISRPN5 IDISRPN6 IDISRPN7 IDISRPN8 /;

# DIS pileup1 on Right HRS
@dis_pileup1_right = qw / IDISRPLN5 IDISRPLW5 IDISRPLN6 IDISRPLW6 IDISRPLN7 IDISRPLW7 IDISRPLN8 IDISRPLW8 /;
# DIS pileup2 on Right HRS
@dis_pileup2_right = qw / IDISRPLN1 IDISRPLW1 IDISRPLN2 IDISRPLW2 IDISRPLN3 IDISRPLW3 IDISRPLN4 IDISRPLW4 /;
# DIS pileup on left HRS
@dis_pileup_left = qw / IDISLPLN1 IDISLPLW1 IDISLPLN2 IDISLPLW2 IDISLPLN3 IDISLPLW3 IDISLPLN4 IDISLPLW4 IDISLPLN5 IDISLPLW5 IDISLPLN6 IDISLPLW6 /;
# DIS mixed1 stuff on Right HRS
@dis_mixed1_right = qw / IDISRENAR IDISREWID IDISRPNAR IDISRPWID IDISRT1 IDISRTAG /;
# DIS mixed2 stuff on Right HRS
@dis_mixed2_right = qw / IDISRSGTAG IDISRSNGTAG IDISRGCTAG IDISRSCTAG IDISRT1ONLY IDISRSP1 IDISRSP2 IDISRSP3 /;
# DIS mixed1 stuff on Left HRS
@dis_mixed1_left = qw / IDISLGCTAG IDISLT1TAG IDISLSGTAG IDISLSNGTAG IDISLENAR IDISLEWID IDISLPNAR IDISLPWID IDISLDELTAG IDISLT2/;
# DIS mixed2 stuff on Left HRS
@dis_mixed2_left = qw / IDISLGC IDISLT1 IDISLMPS IDISLCLK IDISLCLK1 IDISLTAG  /;
# DIS spare channels on Right HRS
@dis_spare_right = qw / IDISRSYNCG1 IDISRSYNCG2 IDISRSYNCG3 IDISRSYNCUG1 IDISRSYNCUG2 IDISRSYNCUG3 IDISRLNE IDISRMPS IDISRT2 IDISRCLK  /;
# DIS spare channels on left HRS
@dis_spare_left = qw / IDISLSYNCG1 IDISLSYNCG2 IDISLSYNCG3 IDISLSYNCUG1 IDISLSYNCUG2 IDISLSYNCUG3 IDISLSP10 IDISLSP11 IDISLSP12 IDISLSP13  /;
# DIS BCM ON RIGHT HRS
@dis_bcm_right = qw / IDISRBCMU1 IDISRBCMU3 IDISRBCMU10 IDISRBCMD1 IDISRBCMD3 IDISRBCMD10 IDISRUNSER IDISRPLN4C /;
# DIS BCM ON LEFT HRS
@dis_bcm_left = qw / IDISLBCMU1 IDISLBCMU3 IDISLBCMU10 IDISLBCMD1 IDISLBCMD3 IDISLBCMD10 IDISLUNSER /;
# DIS total shower copy on Left HRS
@dis_totshC_left = qw / IDISLTS1C IDISLTS2C IDISLTS3C IDISLTS4C IDISLTS5C IDISLTS6C /;
# DIS electron narrow copy on Right HRS
@dis_electnarC_right = qw / IDISREN1C IDISREN2C IDISREN3C IDISREN4C IDISREN5C IDISREN6C IDISREN7C IDISREN8C /;
# DIS preshower copy on Left HRS
@dis_preshC_left = qw / IDISLPS1C IDISLPS2C IDISLPS3C IDISLPS4C IDISLPS5C IDISLPS6C /;
# DIS preshower copy on Right HRS
@dis_electwidC_right = qw / IDISREW1C IDISREW2C IDISREW3C IDISREW4C IDISREW5C IDISREW6C IDISREW7C IDISREW8C /;
# DIS electron narrow copy on Left HRS
@dis_electnarC_left = qw / IDISLEN1C IDISLEN2C IDISLEN3C IDISLEN4C IDISLEN5C IDISLEN6C /;
# DIS electron wide copy on Left HRS
@dis_electwidC_left = qw / IDISLEW1C IDISLEW2C IDISLEW3C IDISLEW4C IDISLEW5C IDISLEW6C /;
# DIS pileup1 copy on Right HRS
@dis_pileup1C_right = qw / IDISRPLN5C IDISRPLW5C IDISRPLN6C IDISRPLW6C IDISRPLN7C IDISRPLW7C IDISRPLN8C IDISRPLW8C /;
# DIS pileup COPY on left HRS
@dis_pileupC_left = qw / IDISLPLN1C IDISLPLW1C IDISLPLN2C IDISLPLW2C IDISLPLN3C IDISLPLW3C IDISLPLN4C IDISLPLW4C IDISLPLN5C IDISLPLW5C IDISLPLN6C IDISLPLW6C /;
# DIS mixed1 stuff copy on Right HRS
@dis_mixed1C_right = qw / IDISRENARC IDISREWIDC IDISRPNARC IDISRPWIDC IDISRT1C IDISRSYNCG2C IDISRTAGC IDISRSYNCUG2C /;
# DIS mixed1 stuff copy on Left HRS
@dis_mixed1C_left = qw / IDISLGCTAGC IDISLT1TAGC IDISLSGTAGC IDISLSNGTAGC IDISLENARC IDISLEWIDC IDISLPNARC IDISLPWIDC  /;
# DIS mixed2 stuff copy on Left HRS
@dis_mixed2C_left = qw / IDISLSYNCG2C IDISLSYNCG1C IDISLSYNCUG1C IDISLCLKC IDISLCLK1C IDISLTAGC  /;

# UMass profile scanner
@prof_list =    qw / IRPROF IRPROFX IRPROFY IRPROFV1 IRPROFV2 IRPROFV3 ILPROF ILPROFX ILPROFY ILPROFV1 ILPROFV2 ILPROFV3 /;
# Batteries
@battlist =      qw / IBATT1 IBATT2 IBATT3 IBATT4 IBATT5 IBATT6 IBATT7 IBATT8/;
# Detectors
@detlist =       qw / IDET1 IDET2 IDET3 IDET4 /;
# Detector combos
@detcomblist =       qw / IDET_L IDET_R IDET_LO IDET_HI IDET_ALL /;
# Lumi detectors
@lumilist =      qw / IBLUMI1 IBLUMI2 IBLUMI3 IBLUMI4 IBLUMI5 IBLUMI6 IBLUMI7 IBLUMI8 IFLUMI1 IFLUMI2 IFLUMI3 ILUMI1 ILUMI2 ILUMI3 ILUMI4/;
# V2F clocks
@v2fclocklist =  qw / IV2F_CLK0 IV2F_CLK1 IV2F_CLK2 IV2F_CLK3 IV2F_CLK4 IV2F_CLK5 IV2F_CLK6 IV2F_CLK7 IV2F_CLK8 IV2F_CLK9 IV2F_CLK10 IV2F_CLK11 IV2F_CLK12 IV2F_CLK13 IV2F_CLK14 /;
# quad photodiode
@qpdlist = qw / IQPD1 /;
# BMW words
@bmwlist = qw / IBMWCLN IBMWOBJ IBMWFREQ IBMWPHASE IBMWPERIOD IBMWAMP IBMWVAL IBMWCYC/;
@bmwcoillist = qw / IBMWCOIL1 IBMWCOIL2 IBMWCOIL3 IBMWCOIL4 IBMWCOIL5 IBMWCOIL6 IBMWCOIL7 IBMWCOIL8/;
# scanflags
@scanflist = qw / ISCANCLEAN ISCANCLEAN1 ISCANCLEAN2 ISCANCLEAN3 ISCANDATA1 ISCANDATA2 ISCANDATA3 ISCANDATA4 ISCANDATA5 ISCANDATA6 ISCANDATA7 ISCANDATA8 ISCANDATA9 ISCANDATA10 ISCANDATA11 ISCANDATA12 /;
# SYNC words
@synclist = qw / IISYNC0 ICHSYNC0 ICHSYNC1 ICHSYNC2 IRSYNC1 IRSYNC2 ILSYNC1 ILSYNC2 /;
# Linear Array
@linalist =      qw / ILINA1 / ;
# Number of old ADC modules, new ADC modules, VQWK modules, scaler modules, and crates
$adcnum = 31;
$vqwknum = 12;
$adcxnum = 31;
$scanum = 15;
$ncrates = 4;

$p = 1;      # next value to be assigned
$out = "";   # output being built

&do_striplines();
&do_cavities();
&do_bcms();
&do_disdet();
&do_distrig();
&do_profile();
&do_batts();
&do_dets();
&do_detcombs();
&do_adcs();
&do_vqwk();
&do_scalers();
&do_tirs();
&do_daqflag();
&do_timeboards();
&do_lumis();
&do_v2fclocks();
&do_qpds();
&do_linas();
&do_bmwwords();
&do_scanflags();
&do_syncwords();

print << "END";
//////////////////////////////////////////////////////////////////////////
//
//  NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE 
//  Note: This header file is generated by the script genDevTypes.pl .
//  Do not edit it; make changes to the script instead.
//  NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE 
//
//////////////////////////////////////////////////////////////////////////
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//           DevTypes.hh  (definition file)
//           ^^^^^^^^^^^
//
//    Authors :  R. Holmes, A. Vacheret, R. Michaels
//
//  Defines arbitrary integer "keys" used to access data (both raw 
//  and calibrated) from fData array in TaEvent class.   
//  The keys have easy-to-remember names.  
//  Restrictions: 
//    1. the keys must be unique.
//    2. MAXKEYS must be > largest key.
//    3. ordering of keys affects TaEvent::Decode().
//
//////////////////////////////////////////////////////////////////////////
 
\#define   MAXKEYS        $p
\#define   ADCREADOUT     1
\#define   SCALREADOUT    2
\#define   TBDREADOUT     3
\#define   TIRREADOUT     4
\#define   DAQFLAG        5
\#define   ADCXREADOUT    6
\#define   VQWKREADOUT    7

// Keys for getting data from devices.
// They keys are mapped to devices automatically.
// Use these keys for  TaEvent::GetData(Int_t key)

END

print $out;

sub do_striplines
{
# Stripline BPMs

    $stroff = $p;
    $strnum = scalar (@halla_strlist) + scalar (@inj_strlist);
    $out1 = "// Hall A striplines\n";
    foreach $str (@halla_strlist)
    {
	$out1 .= &add_str ($str);
    }
    $out1 .= "// Injector striplines\n";
    foreach $str (@inj_strlist)
    {
	$out1 .= &add_str ($str);
    }
    
    $out .= << "ENDSTRCOM";
// Stripline BPMs

\// Stripline BPMs start here
\#define   STROFF     $stroff
\// number of striplines defined below
\#define   STRNUM     $strnum

// XP, XM, YP, YM = antennas; X, Y = calibrated position;
// XWS, YWS, WS = X, Y, and total wiresum

$out1
ENDSTRCOM

    $strcorroff = $p;
    $out1 = "// Hall A stripline wires (before pedestal subtraction)\n";
    foreach $str (@halla_strlist)
    {
	$out1 .= &add_strcorr ($str);
    }
    $out1 .= "// Injector stripline wires (before pedestal subtraction)\n";
    foreach $str (@inj_strlist)
    {
	$out1 .= &add_strcorr ($str);
    }
    
    $out .= << "ENDSTRCORRCOM";
// Stripline wires before pedestal subtraction

\// Corrected Stripline wires
\#define   STRCORROFF     $strcorroff

// XPC, XMC, YPC, YMC = corrected antennas (before pedestal subtraction);

$out1
ENDSTRCORRCOM

}

sub add_str
{
    my ($str) = @_;
    my ($ret);
    $ret = "";
    $ret .= "\#define   ${str}XP    $p\n"; $p++;
    $ret .= "\#define   ${str}XM    $p\n"; $p++;
    $ret .= "\#define   ${str}YP    $p\n"; $p++;
    $ret .= "\#define   ${str}YM    $p\n"; $p++;
    $ret .= "\#define   ${str}X     $p\n"; $p++;
    $ret .= "\#define   ${str}Y     $p\n"; $p++;
    $ret .= "\#define   ${str}XWS   $p\n"; $p++;
    $ret .= "\#define   ${str}YWS   $p\n"; $p++;
    $ret .= "\#define   ${str}WS    $p\n"; $p++;
    $ret .= "\#define   ${str}MX    $p\n"; $p++;
    $ret .= "\n";
    return $ret;
}

sub add_strcorr
{
# Stripline wires.. after dacnoise subtraction or clock division
    my ($str) = @_;
    my ($ret);
    $ret = "";
    $ret .= "\#define   ${str}XPC    $p\n"; $p++;
    $ret .= "\#define   ${str}XMC    $p\n"; $p++;
    $ret .= "\#define   ${str}YPC    $p\n"; $p++;
    $ret .= "\#define   ${str}YMC    $p\n"; $p++;
    $ret .= "\n";
    return $ret;
}

sub do_cavities
{
# Cavity BPMs

    $cavoff = $p;
    $cavnum = scalar (@cavlist);
    $out1 = "";
    foreach $cav (@cavlist)
    {
	$out1 .= &add_cav ($cav);
    }
    
    $out .= << "ENDCAVCOM";
// Cavity BPMs
\// Cavity BPMs start here
\#define   CAVOFF     $cavoff
\// number of cavities defined below
\#define   CAVNUM     $cavnum

// XR, YR = raw data; X, Y = calibrated position

$out1
ENDCAVCOM

    $cavcorroff = $p;
    $out1 = "";
    foreach $cavcorr (@cavlist)
    {
	$out1 .= &add_cavcorr ($cavcorr);
    }
    
    $out .= << "ENDCAVCORRCOM";
// Cavity BPMs (before pedestal subtraction)
\// Cavity BPMs (before peds)
\#define   CAVCORROFF     $cavcorroff

// XC, YC = data before pedestal subtraction

$out1
ENDCAVCORRCOM

}

sub add_cav
{
    my ($cav) = @_;
    my ($ret);
    $ret = "";
    $ret .= "\#define   ${cav}XR    $p\n"; $p++;
    $ret .= "\#define   ${cav}YR    $p\n"; $p++;
    $ret .= "\#define   ${cav}X     $p\n"; $p++;
    $ret .= "\#define   ${cav}Y     $p\n"; $p++;
    $ret .= "\n";
    return $ret;
}

sub add_cavcorr
{
    my ($cav) = @_;
    my ($ret);
    $ret = "";
    $ret .= "\#define   ${cav}XC    $p\n"; $p++;
    $ret .= "\#define   ${cav}YC    $p\n"; $p++;
    $ret .= "\n";
    return $ret;
}

sub do_disdet
{
    $disoff = $p;
    $disnum = scalar (@dis_totsh_right);
    $out1 = "";
    foreach $dis (@dis_totsh_right)
    {
	$out1 .= &add_disdet ($dis);
    }
    $out .= << "ENDDIS1COM";
// DIS TOTAL SHOWER R-HRS

\#define   DISTSROFF     $disoff
\// Number of Total showser on R-HRS
\#define   DISTSRNUM     $disnum

// R = raw data; "" = calibrated data

$out1
ENDDIS1COM

    $disoff = $p;
    $disnum = scalar (@dis_presh_right);
    $out1 = "";
    foreach $dis (@dis_presh_right)
    {
	$out1 .= &add_disdet ($dis);
    }
    $out .= << "ENDDIS1COM";
// DIS PRESHOWER R-HRS

\#define   DISPSROFF     $disoff
\// Number of Preshower on R-HRS
\#define   DISPSRNUM     $disnum

// R = raw data; "" = calibrated data

$out1
ENDDIS1COM

    $disoff = $p;
    $disnum = scalar (@dis_totsh_left);
    $out1 = "";
    foreach $dis (@dis_totsh_left)
    {
	$out1 .= &add_disdet ($dis);
    }
    $out .= << "ENDDIS1COM";
// DIS TOTAL SHOWER L-HRS

\#define   DISTSLOFF     $disoff
\// Number of Total shower on L-HRS
\#define   DISTSLNUM     $disnum

// R = raw data; "" = calibrated data

$out1
ENDDIS1COM

    $disoff = $p;
    $disnum = scalar (@dis_presh_left);
    $out1 = "";
    foreach $dis (@dis_presh_left)
    {
	$out1 .= &add_disdet ($dis);
    }
    $out .= << "ENDDIS1COM";
// DIS PRESHOWER L-HRS

\#define   DISPSLOFF     $disoff
\// Number of Preshower on L-HRS
\#define   DISPSLNUM     $disnum

// R = raw data; "" = calibrated data

$out1
ENDDIS1COM

}


sub do_distrig
{
    $distrigenoff = $p;
    $disnum = scalar (@dis_electnar_left);
    $out1 = "";
    foreach $distrig (@dis_electnar_left)
    {
	$out1 .= &add_distrig ($distrig);
    }
    $out .= << "ENDDISTRIG1COM";
// DIS ELECTRON NARROW L-HRS

\#define   DISTRIGENLOFF     $distrigenoff
\// Number of EN on L-HRS
\#define   DISTRIGENLNUM     $disnum

// R = raw data; "" = calibrated data

$out1
ENDDISTRIG1COM

   $distrigenoff = $p;
    $disnum = scalar (@dis_electnar_right);
    $out1 = "";
    foreach $distrig (@dis_electnar_right)
    {
	$out1 .= &add_distrig ($distrig);
    }
    $out .= << "ENDDISTRIG1COM";
// DIS ELECTRON NARROW R-HRS

\#define   DISTRIGENROFF     $distrigenoff
\// Number of EN on R-HRS
\#define   DISTRIGENRNUM     $disnum

// R = raw data; "" = calibrated data

$out1
ENDDISTRIG1COM

$distrigewoff = $p;
    $disnum = scalar (@dis_electwid_left);
    $out1 = "";
    foreach $distrig (@dis_electwid_left)
    {
	$out1 .= &add_distrig ($distrig);
    }
    $out .= << "ENDDISTRIG1COM";
// DIS ELECTRON WIDE L-HRS

\#define   DISTRIGEWLOFF     $distrigewoff
\// Number of EW on L-HRS
\#define   DISTRIGEWLNUM     $disnum

// R = raw data; "" = calibrated data

$out1
ENDDISTRIG1COM

   $distrigewoff = $p;
    $disnum = scalar (@dis_electwid_right);
    $out1 = "";
    foreach $distrig (@dis_electwid_right)
    {
	$out1 .= &add_distrig ($distrig);
    }
    $out .= << "ENDDISTRIG1COM";
// DIS ELECTRON WIDE R-HRS

\#define   DISTRIGEWROFF     $distrigewoff
\// Number of EW on R-HRS
\#define   DISTRIGEWRNUM     $disnum

// R = raw data; "" = calibrated data

$out1
ENDDISTRIG1COM

$distrigpwoff = $p;
    $disnum = scalar (@dis_pionwid_left);
    $out1 = "";
    foreach $distrig (@dis_pionwid_left)
    {
	$out1 .= &add_distrig ($distrig);
    }
    $out .= << "ENDDISTRIG1COM";
// DIS PION WIDE L-HRS

\#define   DISTRIGPWLOFF     $distrigpwoff
\// Number of PW on L-HRS
\#define   DISTRIGPWLNUM     $disnum

// R = raw data; "" = calibrated data

$out1
ENDDISTRIG1COM

   $distrigpwoff = $p;
    $disnum = scalar (@dis_pionwid_right);
    $out1 = "";
    foreach $distrig (@dis_pionwid_right)
    {
	$out1 .= &add_distrig ($distrig);
    }
    $out .= << "ENDDISTRIG1COM";
// DIS PION WIDE R-HRS

\#define   DISTRIGPWROFF     $distrigpwoff
\// Number of PW on R-HRS
\#define   DISTRIGPWRNUM     $disnum

// R = raw data; "" = calibrated data

$out1
ENDDISTRIG1COM
    

 $distrigpnoff = $p;
    $disnum = scalar (@dis_pionnar_left);
    $out1 = "";
    foreach $distrig (@dis_pionnar_left)
    {
	$out1 .= &add_distrig ($distrig);
    }
    $out .= << "ENDDISTRIG1COM";
// DIS PION NARROW L-HRS

\#define   DISTRIGPNLOFF     $distrigpnoff
\// Number of PN on L-HRS
\#define   DISTRIGPNLNUM     $disnum

// R = raw data; "" = calibrated data

$out1
ENDDISTRIG1COM

   $distrigpnoff = $p;
    $disnum = scalar (@dis_pionnar_right);
    $out1 = "";
    foreach $distrig (@dis_pionnar_right)
    {
	$out1 .= &add_distrig ($distrig);
    }
    $out .= << "ENDDISTRIG1COM";
// DIS PION NARROW R-HRS

\#define   DISTRIGPNROFF     $distrigpnoff
\// Number of PN on R-HRS
\#define   DISTRIGPNRNUM     $disnum

// R = raw data; "" = calibrated data

$out1
ENDDISTRIG1COM

$distrigpl1roff = $p;
    $disnum = scalar (@dis_pileup1_right);
    $out1 = "";
    foreach $distrig (@dis_pileup1_right)
    {
	$out1 .= &add_distrig ($distrig);
    }
    $out .= << "ENDDISTRIG1COM";
// DIS PILEUP grp 5-8 R-HRS

\#define   DISTRIGPL1ROFF     $distrigpl1roff
\// Number of PL on R-HRS
\#define   DISTRIGPL1RNUM     $disnum

// R = raw data; "" = calibrated data

$out1
ENDDISTRIG1COM

$distrigmxd1roff = $p;
    $disnum = scalar (@dis_mixed1_right);
    $out1 = "";
    foreach $distrig (@dis_mixed1_right)
    {
	$out1 .= &add_distrig ($distrig);
    }
    $out .= << "ENDDISTRIG1COM";
// DIS MIXED STUFF R-HRS

\#define   DISTRIGMXD1ROFF     $distrigmxd1roff
\// Number of MIXD on R-HRS
\#define   DISTRIGMXD1RNUM     $disnum

// R = raw data; "" = calibrated data

$out1
ENDDISTRIG1COM

$distrigmxd2roff = $p;
    $disnum = scalar (@dis_mixed2_right);
    $out1 = "";
    foreach $distrig (@dis_mixed2_right)
    {
	$out1 .= &add_distrig ($distrig);
    }
    $out .= << "ENDDISTRIG1COM";
// DIS MIXED 2 STUFF R-HRS

\#define   DISTRIGMXD2ROFF     $distrigmxd2roff
\// Number of MIXD 2 on R-HRS
\#define   DISTRIGMXD2RNUM     $disnum

// R = raw data; "" = calibrated data

$out1
ENDDISTRIG1COM

 $distrigmxd1loff = $p;
    $disnum = scalar (@dis_mixed1_left);
    $out1 = "";
    foreach $distrig (@dis_mixed1_left)
    {
	$out1 .= &add_distrig ($distrig);
    }
    $out .= << "ENDDISTRIG1COM";
// DIS MIXED STUFF L-HRS

\#define   DISTRIGMXD1LOFF     $distrigmxd1loff
\// Number of MIXD on L-HRS
\#define   DISTRIGMXD1LNUM     $disnum

// R = raw data; "" = calibrated data

$out1
ENDDISTRIG1COM

$distrigmxd2loff = $p;
    $disnum = scalar (@dis_mixed2_left);
    $out1 = "";
    foreach $distrig (@dis_mixed2_left)
    {
	$out1 .= &add_distrig ($distrig);
    }
    $out .= << "ENDDISTRIG1COM";
// DIS MIXED 2 STUFF L-HRS

\#define   DISTRIGMXD2LOFF     $distrigmxd2loff
\// Number of MIXD 2 on L-HRS
\#define   DISTRIGMXD2LNUM     $disnum

// R = raw data; "" = calibrated data

$out1
ENDDISTRIG1COM

 $distrigpl2roff = $p;
    $disnum = scalar (@dis_pileup2_right);
    $out1 = "";
    foreach $distrig (@dis_pileup2_right)
    {
	$out1 .= &add_distrig ($distrig);
    }
    $out .= << "ENDDISTRIG1COM";
// DIS PILEUP grp 1-4 R-HRS

\#define   DISTRIGPL2ROFF     $distrigpl2roff
\// Number of PL on R-HRS
\#define   DISTRIGPL2RNUM     $disnum

// R = raw data; "" = calibrated data

$out1
ENDDISTRIG1COM

$distrigplloff = $p;
    $disnum = scalar (@dis_pileup_left);
    $out1 = "";
    foreach $distrig (@dis_pileup_left)
    {
	$out1 .= &add_distrig ($distrig);
    }
    $out .= << "ENDDISTRIG1COM";
// DIS PILEUP grp 1-6 l-HRS

\#define   DISTRIGPLLOFF     $distrigplloff
\// Number of PL on L-HRS
\#define   DISTRIGPLLNUM     $disnum

// R = raw data; "" = calibrated data

$out1
ENDDISTRIG1COM

$distrigsproff = $p;
    $disnum = scalar (@dis_spare_right);
    $out1 = "";
    foreach $distrig (@dis_spare_right)
    {
	$out1 .= &add_distrig ($distrig);
    }
    $out .= << "ENDDISTRIG1COM";
// DIS spare channels R-HRS

\#define   DISTRIGSPROFF     $distrigsproff
\// Number of spare channels on R-HRS
\#define   DISTRIGSPRNUM     $disnum

// R = raw data; "" = calibrated data

$out1
ENDDISTRIG1COM

$distrigsploff = $p;
    $disnum = scalar (@dis_spare_left);
    $out1 = "";
    foreach $distrig (@dis_spare_left)
    {
	$out1 .= &add_distrig ($distrig);
    }
    $out .= << "ENDDISTRIG1COM";
// DIS spare channels L-HRS

\#define   DISTRIGSPLOFF     $distrigsploff
\// Number of spare channels on L-HRS
\#define   DISTRIGSPLNUM     $disnum

// R = raw data; "" = calibrated data

$out1
ENDDISTRIG1COM

 $distrigbcmroff = $p;
    $disnum = scalar (@dis_bcm_right);
    $out1 = "";
    foreach $distrig (@dis_bcm_right)
    {
	$out1 .= &add_distrig ($distrig);
    }
    $out .= << "ENDDISTRIG1COM";
// DIS BCM R-HRS

\#define   DISTRIGBCMROFF     $distrigbcmroff
\// Number of bcm on R-HRS
\#define   DISTRIGBCMRNUM     $disnum

// R = raw data; "" = calibrated data

$out1
ENDDISTRIG1COM

$distrigbcmloff = $p;
    $disnum = scalar (@dis_bcm_left);
    $out1 = "";
    foreach $distrig (@dis_bcm_left)
    {
	$out1 .= &add_distrig ($distrig);
    }
    $out .= << "ENDDISTRIG1COM";
// DIS BCM L-HRS

\#define   DISTRIGBCMLOFF     $distrigbcmloff
\// Number of bcm on L-HRS
\#define   DISTRIGBCMLNUM     $disnum

// R = raw data; "" = calibrated data

$out1
ENDDISTRIG1COM

 $distrigtscoff = $p;
    $disnum = scalar (@dis_totshC_left);
    $out1 = "";
    foreach $distrig (@dis_totshC_left)
    {
	$out1 .= &add_distrig ($distrig);
    }
    $out .= << "ENDDISTRIG1COM";
// DIS TOTAL SHOWER COPY L-HRS

\#define   DISTRIGTSCLOFF     $distrigtscoff
\// Number of TSC on L-HRS
\#define   DISTRIGTSCLNUM     $disnum

// R = raw data; "" = calibrated data

$out1
ENDDISTRIG1COM

  $distrigencoff = $p;
    $disnum = scalar (@dis_electnarC_right);
    $out1 = "";
    foreach $distrig (@dis_electnarC_right)
    {
	$out1 .= &add_distrig ($distrig);
    }
    $out .= << "ENDDISTRIG1COM";
// DIS ELECTRON NARROW COPY R-HRS

\#define   DISTRIGENCROFF     $distrigencoff
\// Number of ENC on R-HRS
\#define   DISTRIGENCRNUM     $disnum

// R = raw data; "" = calibrated data

$out1
ENDDISTRIG1COM


 $distrigpscoff = $p;
    $disnum = scalar (@dis_preshC_left);
    $out1 = "";
    foreach $distrig (@dis_preshC_left)
    {
	$out1 .= &add_distrig ($distrig);
    }
    $out .= << "ENDDISTRIG1COM";
// DIS PRESHOWER COPY L-HRS

\#define   DISTRIGPSCLOFF     $distrigpscoff
\// Number of PSC on L-HRS
\#define   DISTRIGPSCLNUM     $disnum

// R = raw data; "" = calibrated data

$out1
ENDDISTRIG1COM

  $distrigewcoff = $p;
    $disnum = scalar (@dis_electwidC_right);
    $out1 = "";
    foreach $distrig (@dis_electwidC_right)
    {
	$out1 .= &add_distrig ($distrig);
    }
    $out .= << "ENDDISTRIG1COM";
// DIS ELECTWID COPY R-HRS

\#define   DISTRIGEWCROFF     $distrigewcoff
\// Number of EWC on R-HRS
\#define   DISTRIGEWCRNUM     $disnum

// R = raw data; "" = calibrated data

$out1
ENDDISTRIG1COM


 $distrigencoff = $p;
    $disnum = scalar (@dis_electnarC_left);
    $out1 = "";
    foreach $distrig (@dis_electnarC_left)
    {
	$out1 .= &add_distrig ($distrig);
    }
    $out .= << "ENDDISTRIG1COM";
// DIS ELECTRON NARROW COPY L-HRS

\#define   DISTRIGENCLOFF     $distrigencoff
\// Number of ENC on L-HRS
\#define   DISTRIGENCLNUM     $disnum

// R = raw data; "" = calibrated data

$out1
ENDDISTRIG1COM

 $distrigewcoff = $p;
    $disnum = scalar (@dis_electwidC_left);
    $out1 = "";
    foreach $distrig (@dis_electwidC_left)
    {
	$out1 .= &add_distrig ($distrig);
    }
    $out .= << "ENDDISTRIG1COM";
// DIS ELECTRON WIDE COPY L-HRS

\#define   DISTRIGEWCLOFF     $distrigewcoff
\// Number of EWC on L-HRS
\#define   DISTRIGEWCLNUM     $disnum

// R = raw data; "" = calibrated data

$out1
ENDDISTRIG1COM

  $distrigplcroff = $p;
    $disnum = scalar (@dis_pileup1C_right);
    $out1 = "";
    foreach $distrig (@dis_pileup1C_right)
    {
	$out1 .= &add_distrig ($distrig);
    }
    $out .= << "ENDDISTRIG1COM";
// DIS pileup gr 5-8 COPY R-HRS

\#define   DISTRIGPL1CROFF     $distrigplcroff
\// Number of pl1C on R-HRS
\#define   DISTRIGPL1CRNUM     $disnum

// R = raw data; "" = calibrated data

$out1
ENDDISTRIG1COM

 $distrigplcloff = $p;
    $disnum = scalar (@dis_pileupC_left);
    $out1 = "";
    foreach $distrig (@dis_pileupC_left)
    {
	$out1 .= &add_distrig ($distrig);
    }
    $out .= << "ENDDISTRIG1COM";
// DIS pileup  COPY L-HRS

\#define   DISTRIGPLCLOFF     $distrigplcloff
\// Number of plC on L-HRS
\#define   DISTRIGPLCLNUM     $disnum

// R = raw data; "" = calibrated data

$out1
ENDDISTRIG1COM

 $distrigmxd1croff = $p;
    $disnum = scalar (@dis_mixed1C_right);
    $out1 = "";
    foreach $distrig (@dis_mixed1C_right)
    {
	$out1 .= &add_distrig ($distrig);
    }
    $out .= << "ENDDISTRIG1COM";
// DIS MIXED1 STUFF COPY R-HRS

\#define   DISTRIGMXD1CROFF     $distrigmxd1croff
\// Number of MIXD1C on R-HRS
\#define   DISTRIGMXD1CRNUM     $disnum

// R = raw data; "" = calibrated data

$out1
ENDDISTRIG1COM
   
$distrigmxd1cloff = $p;
    $disnum = scalar (@dis_mixed1C_left);
    $out1 = "";
    foreach $distrig (@dis_mixed1C_left)
    {
	$out1 .= &add_distrig ($distrig);
    }
    $out .= << "ENDDISTRIG1COM";
// DIS MIXED STUFF L-HRS

\#define   DISTRIGMXD1CLOFF     $distrigmxd1cloff
\// Number of MIXD COPY on L-HRS
\#define   DISTRIGMXD1CLNUM     $disnum

// R = raw data; "" = calibrated data

$out1
ENDDISTRIG1COM

$distrigmxd2cloff = $p;
    $disnum = scalar (@dis_mixed2C_left);
    $out1 = "";
    foreach $distrig (@dis_mixed2C_left)
    {
	$out1 .= &add_distrig ($distrig);
    }
    $out .= << "ENDDISTRIG1COM";
// DIS MIXED 2 COPY STUFF L-HRS

\#define   DISTRIGMXD2CLOFF     $distrigmxd2cloff
\// Number of MIXD 2 COPY on L-HRS
\#define   DISTRIGMXD2CLNUM     $disnum

// R = raw data; "" = calibrated data

$out1
ENDDISTRIG1COM

}


sub do_profile
{
# UMass profile scanner

    $profoff = $p;
    $profnum = scalar (@prof_list);
    $out1 = "";
    foreach $prof (@prof_list)
    {
	$out1 .= &add_profile ($prof);
    }
    
    $out .= << "ENDPROFCOM";
// UMass Profile Scanner
\// UMass profile scanner start here
\#define   PROFOFF     $profoff
\// num. prof. devices defined below
\#define   PROFNUM     $profnum

// "PROFX", "PROFY" = positions, "PROF" = amplitude.
// "PROFV1" "V2" "V3" = control voltage levels.
// Prefix "IR","IL" = right, left spectrometer.
// Suffix "R" = raw data.
// Suffix "C" = corrected but no pedestal subtraction.
// No suffix = fully corrected data.

$out1
ENDPROFCOM
}

sub add_profile
{
    my ($prof) = @_;
    my ($ret);
    $ret = "";
    $ret .= "\#define   ${prof}R    $p\n"; $p++;
    $ret .= "\#define   ${prof}C    $p\n"; $p++;
    $ret .= "\#define   ${prof}    $p\n"; $p++;
    $ret .= "\n";
    return $ret;
}


sub do_bcms
{
# BCMs

    $bcmoff = $p;
    $bcmnum = scalar (@old_bcmlist);
    $out1 = "";
    foreach $bcm (@old_bcmlist)
    {
	$out1 .= &add_bcm ($bcm);
    }
    $out .= << "ENDBCMCOM";
// Old (HAPPEX-I era) BCMs

\// Old BCMs start here
\#define   BCMOFF     $bcmoff
\// number of old BCMs defined below
\#define   BCMNUM     $bcmnum

// R = raw data; "" = calibrated data

$out1
ENDBCMCOM

    $bcmcorroff = $p;
    $out1 = "";
    foreach $bcm (@old_bcmlist)
    {
	$out1 .= &add_bcmcorr ($bcm);
    }
    $out .= << "ENDBCMCORRCOM";
// Old (HAPPEX-I era) BCMs (before pedestal subtraction)

\// Old BCMs start here
\#define   BCMCORROFF     $bcmcorroff

// C = data before pedestal subtraction

$out1
ENDBCMCORRCOM

    $ccmoff = $p;
    $ccmnum = scalar (@g0_bcmlist);
    $out1 = "";
    foreach $bcm (@g0_bcmlist)
    {
	$out1 .= &add_bcm ($bcm);
    }
    
    $out .= << "ENDCCMCOM";
// G0 BCMs

\// G0 BCMs start here
\#define   CCMOFF     $ccmoff
\// number of G0 BCMs defined below
\#define   CCMNUM     $ccmnum

// R = raw data; "" = calibrated data

$out1
ENDCCMCOM

    $ccmcorroff = $p;
    $out1 = "";
    foreach $bcm (@g0_bcmlist)
    {
	$out1 .= &add_bcmcorr ($bcm);
    }
    
    $out .= << "ENDCCMCORRCOM";
// G0 BCMs (before pedestal subtraction)

\// G0 BCMs (before peds)
\#define   CCMCORROFF     $ccmcorroff

// C = data before pedestal subtraction

$out1
ENDCCMCORRCOM
}

sub add_bcm
{
    my ($bcm) = @_;
    my ($ret);
    $ret = "";
    $ret .= "\#define   ${bcm}R     $p\n"; $p++;
    $ret .= "\#define   ${bcm}      $p\n"; $p++;
    $ret .= "\n";
    return $ret;
}

sub add_bcmcorr
{
    my ($bcm) = @_;
    my ($ret);
    $ret = "";
    $ret .= "\#define   ${bcm}C     $p\n"; $p++;
    $ret .= "\n";
    return $ret;
}


sub add_disdet
{
    my ($disdet) = @_;
    my ($ret);
    $ret = "";
    $ret .= "\#define   ${disdet}R     $p\n"; $p++;
    $ret .= "\#define   ${disdet}      $p\n"; $p++;
    $ret .= "\n";
    return $ret;
}

sub add_distrig
{
    my ($distrig) = @_;
    my ($ret);
    $ret = "";
    $ret .= "\#define   ${distrig}R     $p\n"; $p++;
    $ret .= "\#define   ${distrig}      $p\n"; $p++;
    $ret .= "\n";
    return $ret;
}


sub do_batts
{
# Batteries

    $batoff = $p;
    $batnum = scalar (@battlist);
    $out1 = "";
    foreach $batt (@battlist)
    {
	$out1 .= &add_batt ($batt);
    }
    $out .= << "ENDBATTCOM";
// Batteries

\// Batteries start here
\#define   BATOFF     $batoff
\// number of batteries defined below
\#define   BATNUM     $batnum

$out1
ENDBATTCOM
}

sub add_batt
{
    my ($batt) = @_;
    my ($ret);
    $ret = "\#define   ${batt}      $p\n"; $p++;
    return $ret;
}

sub do_dets
{
# Detectors

    $detoff = $p;
    $detnum = scalar (@detlist);
    $out1 = "";
    foreach $det (@detlist)
    {
	$out1 .= &add_det ($det);
    }
    $out .= << "ENDDETCOM";
// Detectors

\// Detectorss start here
\#define   DETOFF     $detoff
\// number of detectors defined below
\#define   DETNUM     $detnum

// R = raw data; "" = calibrated data

$out1
ENDDETCOM

    $detcorroff = $p;
    $out1 = "";
    foreach $det (@detlist)
    {
	$out1 .= &add_detcorr ($det);
    }
    $out .= << "ENDDETCORRCOM";
// Detectors (before pedestal subtraction)

\// Detectors (before peds)
\#define   DETCORROFF     $detcorroff

// C = data before pedestal subtraction

$out1
ENDDETCORRCOM
}

sub add_det
{
    my ($det) = @_;
    my ($ret);
    $ret = "";
    $ret .= "\#define   ${det}R     $p\n"; $p++;
    $ret .= "\#define   ${det}      $p\n"; $p++;
    $ret .= "\#define   ${det}S      $p\n"; $p++;
    $ret .= "\n";
    return $ret;
}

sub add_detcorr
{
    my ($det) = @_;
    my ($ret);
    $ret = "";
    $ret .= "\#define   ${det}C     $p\n"; $p++;
    $ret .= "\n";
    return $ret;
}


sub do_detcombs
{
# Detector Combinations

    $detcomboff = $p;
    $detcombnum = scalar (@detcomblist);
    $out1 = "";
    foreach $detcomb (@detcomblist)
    {
	$out1 .= &add_detcomb ($detcomb);
    }
    $out .= << "ENDDETCOMBCOM";
// Detector Combinations

\// Detector Combinations start here
\#define   DETCOMBOFF     $detcomboff
\// number of detector combos defined below
\#define   DETCOMBNUM     $detcombnum


$out1
ENDDETCOMBCOM
}

sub add_detcomb
{
    my ($detcomb) = @_;
    my ($ret);
    $ret = "";
    $ret .= "\#define   ${detcomb}      $p\n"; $p++;
    $ret .= "\n";
    return $ret;
}


sub do_adcs
{
# ADCs -- old and new.  We guarantee new ADC quantities will
# immediately follow corresponding old ADC quantities, so one can loop
# over both.

    $adcoff = $p;
    $adcxoff = $p + $adcnum * 4;
    $out1 = "// Here are the old ADCs\n";
    for $iadc (0..$adcnum-1)
    {
	$adc = "IADC$iadc";
	$out1 .= &add_adc ($adc);
    }
    $out1 .= "// Here are the new (18 bit) ADCs\n";
    for $iadcx (0..$adcxnum-1)
    {
	$adcx = "IADCX$iadcx";
	$out1 .= &add_adc ($adcx);
    }
    $out .= << "ENDADCCOM";
\// ADC data.  Data are arranged in sequence starting with ADC0_0
\// First index is the adc#, second is channel#.  Indices start at 0
\// ADC# 0 - 9 are in first crate, 10 in next crate, etc.  Both old
\// ADCs (type adc) and new (type adcx) are included.

\// First the raw data

\// Raw ADCs start here
\#define   ADCOFF     $adcoff
\#define   ADCXOFF   $adcxoff
\// number of ADCs defined below
\#define   ADCNUM     $adcnum
\#define   ADCXNUM   $adcxnum

$out1
ENDADCCOM

    $adcdacsuboff = $p;
    $adcxdacsuboff = $p + $adcnum * 4;
    $out1 = "// Here are the old ADCs\n";
    for $iadcdacsub (0..$adcnum-1)
    {
	$adcdacsub = "IADC$iadcdacsub";
	$out1 .= &add_adcdacsub ($adcdacsub);
    }
    $out1 .= "// Here are the new (18 bit) ADCs\n";
    for $iadcxdacsub (0..$adcxnum-1)
    {
	$adcxdacsub = "IADCX$iadcxdacsub";
	$out1 .= &add_adcdacsub ($adcxdacsub);
    }
    $out .= << "ENDADCDACSUBCOM";
// Now the dacnoise subtracted data

\// Dacnoise Subtracted ADCs start here
\#define   ADCDACSUBOFF     $adcdacsuboff
\#define   ADCXDACSUBOFF   $adcxdacsuboff

$out1
ENDADCDACSUBCOM

    $accoff = $p;
    $accxoff = $p + $adcnum * 4;
    $out1 = "// Here are the old ADCs\n";
    for $iacc (0..$adcnum-1)
    {
	$acc = "IADC$iacc";
	$out1 .= &add_adccal ($acc);
    }
    $out1 .= "// Here are the new (18 bit) ADCs\n";
    for $iaccx (0..$adcxnum-1)
    {
	$accx = "IADCX$iaccx";
	$out1 .= &add_adccal ($accx);
    }
    $out .= << "ENDACCCOM";
// Now the calibrated data

\// Calibrated ADCs start here
\#define   ACCOFF     $accoff
\#define   ACCXOFF   $accxoff

$out1
ENDACCCOM

# New ADCs only: Baseline samples

    $adcxbsoff = $p;
    $out1 = "";
    for $iadcxbs (0..$adcxnum-1)
    {
	$adcxbs = "IADCX$iadcxbs";
	$out1 .= &add_adcbs ($adcxbs);
    }
    $out .= << "ENDADCBSCOM";
// Now the baseline sample data

\// ADCX baseline samples start here
\#define   ADCXBSOFF   $adcxbsoff

$out1
ENDADCBSCOM

# New ADCs only: Baseline samples

    $adcxpsoff = $p;
    $out1 = "";
    for $iadcxps (0..$adcxnum-1)
    {
	$adcxps = "IADCX$iadcxps";
	$out1 .= &add_adcps ($adcxps);
    }
    $out .= << "ENDADCPSCOM";
// Now the peak sample data

\// ADCX baseline samples start here
\#define   ADCXPSOFF   $adcxpsoff

$out1
ENDADCPSCOM

    $dacoff = $p;
    $dacnum = $adcnum;
    $dacxoff = $p + $dacnum;
    $dacxnum = $adcxnum;
    $out1 = "// Here are the old ADCs\n";
    for $idac (0..$dacnum-1)
    {
	$dac = "IDAC$idac";
	$out1 .= &add_daccsr ($dac);
    }
    $out1 .= "// Here are the new (18 bit) ADCs\n";
    for $idacx (0..$dacxnum-1)
    {
	$dacx = "IDACX$idacx";
	$out1 .= &add_daccsr ($dacx);
    }
    $out .= << "ENDDACCOM";
// DAC noise.

\// DACs start here
\#define   DACOFF     $dacoff
\#define   DACXOFF     $dacxoff
\// number of DACs defined below
\#define   DACNUM     $dacnum
\#define   DACXNUM     $dacxnum

$out1
ENDDACCOM

    $csroff = $p;
    $csrnum = $adcnum;
    $csrxoff = $p + $csrnum;
    $csrxnum = $adcxnum;
    $out1 = "// Here are the old ADCs\n";
    for $icsr (0..$csrnum-1)
    {
	$csr = "ICSR$icsr";
	$out1 .= &add_daccsr ($csr);
    }
    $out1 .= "// Here are the new (18 bit) ADCs\n";
    for $icsrx (0..$csrxnum-1)
    {
	$csrx = "ICSRX$icsrx";
	$out1 .= &add_daccsr ($csrx);
    }
    $out .= << "ENDCSRCOM";
// CSRs

\// CSRs start here
\#define   CSROFF     $csroff
\#define   CSRXOFF     $csrxoff
\// number of CSRs defined below
\#define   CSRNUM     $csrnum
\#define   CSRXNUM     $csrxnum

$out1
ENDCSRCOM
}

sub add_adc
{
    my ($adc) = @_;
    my ($ret);
    $ret = "";
    $ret .= "\#define   ${adc}_0     $p\n"; $p++;
    $ret .= "\#define   ${adc}_1     $p\n"; $p++;
    $ret .= "\#define   ${adc}_2     $p\n"; $p++;
    $ret .= "\#define   ${adc}_3     $p\n"; $p++;
    return $ret;
}

sub add_adcdacsub
{
    my ($adc) = @_;
    my ($ret);
    $ret = "";
    $ret .= "\#define   ${adc}_0_DACSUB    $p\n"; $p++;
    $ret .= "\#define   ${adc}_1_DACSUB    $p\n"; $p++;
    $ret .= "\#define   ${adc}_2_DACSUB    $p\n"; $p++;
    $ret .= "\#define   ${adc}_3_DACSUB    $p\n"; $p++;
    return $ret;
}

sub add_adccal
{
    my ($adc) = @_;
    my ($ret);
    $ret = "";
    $ret .= "\#define   ${adc}_0_CAL     $p\n"; $p++;
    $ret .= "\#define   ${adc}_1_CAL     $p\n"; $p++;
    $ret .= "\#define   ${adc}_2_CAL     $p\n"; $p++;
    $ret .= "\#define   ${adc}_3_CAL     $p\n"; $p++;
    return $ret;
}

sub add_adcbs
{
    my ($adc) = @_;
    my ($ret);
    $ret = "";
    $ret .= "\#define   ${adc}_0_B0     $p\n"; $p++;
    $ret .= "\#define   ${adc}_0_B1     $p\n"; $p++;
    $ret .= "\#define   ${adc}_0_B2     $p\n"; $p++;
    $ret .= "\#define   ${adc}_0_B3     $p\n"; $p++;
    $ret .= "\#define   ${adc}_1_B0     $p\n"; $p++;
    $ret .= "\#define   ${adc}_1_B1     $p\n"; $p++;
    $ret .= "\#define   ${adc}_1_B2     $p\n"; $p++;
    $ret .= "\#define   ${adc}_1_B3     $p\n"; $p++;
    $ret .= "\#define   ${adc}_2_B0     $p\n"; $p++;
    $ret .= "\#define   ${adc}_2_B1     $p\n"; $p++;
    $ret .= "\#define   ${adc}_2_B2     $p\n"; $p++;
    $ret .= "\#define   ${adc}_2_B3     $p\n"; $p++;
    $ret .= "\#define   ${adc}_3_B0     $p\n"; $p++;
    $ret .= "\#define   ${adc}_3_B1     $p\n"; $p++;
    $ret .= "\#define   ${adc}_3_B2     $p\n"; $p++;
    $ret .= "\#define   ${adc}_3_B3     $p\n"; $p++;
    return $ret;
}

sub add_adcps
{
    my ($adc) = @_;
    my ($ret);
    $ret = "";
    $ret .= "\#define   ${adc}_0_P0     $p\n"; $p++;
    $ret .= "\#define   ${adc}_0_P1     $p\n"; $p++;
    $ret .= "\#define   ${adc}_0_P2     $p\n"; $p++;
    $ret .= "\#define   ${adc}_0_P3     $p\n"; $p++;
    $ret .= "\#define   ${adc}_1_P0     $p\n"; $p++;
    $ret .= "\#define   ${adc}_1_P1     $p\n"; $p++;
    $ret .= "\#define   ${adc}_1_P2     $p\n"; $p++;
    $ret .= "\#define   ${adc}_1_P3     $p\n"; $p++;
    $ret .= "\#define   ${adc}_2_P0     $p\n"; $p++;
    $ret .= "\#define   ${adc}_2_P1     $p\n"; $p++;
    $ret .= "\#define   ${adc}_2_P2     $p\n"; $p++;
    $ret .= "\#define   ${adc}_2_P3     $p\n"; $p++;
    $ret .= "\#define   ${adc}_3_P0     $p\n"; $p++;
    $ret .= "\#define   ${adc}_3_P1     $p\n"; $p++;
    $ret .= "\#define   ${adc}_3_P2     $p\n"; $p++;
    $ret .= "\#define   ${adc}_3_P3     $p\n"; $p++;
    return $ret;
}

sub add_daccsr
{
    my ($adc) = @_;
    my ($ret);
    $ret = "\#define   ${adc}      $p\n"; $p++;
    return $ret;
}


sub do_vqwk
{
# VQWK -- Qweak voltage integrators. 

    $vqwkoff = $p;
    $out1 = "// Here are the Qweak Integrators\n";
    for $ivqw (0..$vqwknum-1)
    {
	$vqwk = "IVQWK$ivqw";
	$out1 .= &add_vqwk ($vqwk);
    }
    $out .= << "ENDVQWKCOM";
\// VQWK data.  Data are arranged in sequence starting with VQWK0_0
\// First index is the VQWK#, second is channel#.  Indices start at 0

\// First the raw data

\// Raw VQWK data start here
\#define   VQWKOFF     $vqwkoff
\// number of VQWKs defined below
\#define   VQWKNUM     $vqwknum

$out1
ENDVQWKCOM


    $vqcoff = $p;
    $out1 = "// Here are the Qweak integrators\n";
    for $ivqc (0..$vqwknum-1)
    {
	$vqwk = "IVQWK$ivqc";
	$out1 .= &add_vqwkcal ($vqwk);
    }
    $out .= << "ENDVQCCOM";
// Now the calibrated data

\// Calibrated ADCs start here
\#define   VQWKCOFF     $vqcoff

$out1
ENDVQCCOM
}

sub add_vqwk
{
    my ($vqwk) = @_;
    my ($ret);
    $ret = "";
    for $i (0..7)
    {
	$ret .= "\#define   ${vqwk}_${i}_NSAMP    $p\n"; $p++;
	$ret .= "\#define   ${vqwk}_${i}_NUM      $p\n"; $p++;
	$ret .= "\#define   ${vqwk}_${i}_B1       $p\n"; $p++;
	$ret .= "\#define   ${vqwk}_${i}_B2       $p\n"; $p++;
	$ret .= "\#define   ${vqwk}_${i}_B3       $p\n"; $p++;
	$ret .= "\#define   ${vqwk}_${i}_B4       $p\n"; $p++;
	$ret .= "\#define   ${vqwk}_${i}          $p\n"; $p++;
    }
    $ret .= "\n";
    return $ret;
}

sub add_vqwkcal
{
    my ($vqwk) = @_;
    my ($ret);
    $ret = "";
    for $i (0..7)
    {
	$ret .= "\#define   ${vqwk}_${i}_B1_CAL    $p\n"; $p++;
	$ret .= "\#define   ${vqwk}_${i}_B2_CAL    $p\n"; $p++;
	$ret .= "\#define   ${vqwk}_${i}_B3_CAL    $p\n"; $p++;
	$ret .= "\#define   ${vqwk}_${i}_B4_CAL    $p\n"; $p++;
	$ret .= "\#define   ${vqwk}_${i}_CAL       $p\n"; $p++;
    }
    $ret .= "\n";
    return $ret;

}



sub do_scalers
{
# Scalers

    $scaoff = $p;
    $out1 = "";
    for $iscaler (0..$scanum-1)
    {
	$scaler = "ISCALER$iscaler";
	$out1 .= &add_scaler ($scaler);
    }
    $out .= << "ENDSCALERCOM";
// Raw scalers

\// Raw SCALERs start here
\#define   SCAOFF     $scaoff
\// number of SCALERs defined below
\#define   SCANUM     $scanum

$out1
ENDSCALERCOM

    $scaclkdivoff = $p;
    $out1 = "";
    for $iscaclkdiv (0..$scanum-1)
    {
	$sclkdiv = "ISCALER$iscaclkdiv";
	$out1 .= &add_scalerclkdiv ($sclkdiv);
    }
    $out .= << "ENDSCACLKDIVCOM";
// Clock Divided scalers (before pedestal subtraction)

\// Clock Divided SCALERs start here
\#define   SCACLKDIVOFF     $scaclkdivoff

$out1
ENDSCACLKDIVCOM

    $sccoff = $p;
    $out1 = "";
    for $iscc (0..$scanum-1)
    {
	$scc = "ISCALER$iscc";
	$out1 .= &add_scalercal ($scc);
    }
    $out .= << "ENDSCCCOM";
// Calibrated scalers

\// Calibrated SCALERs start here
\#define   SCCOFF     $sccoff     

$out1
ENDSCCCOM
}

sub add_scaler
{
    my ($scaler) = @_;
    my ($ret);
    $ret = "";
    for $i (0..31)
    {
	$ret .= "\#define   ${scaler}_$i     $p\n"; $p++;
    }
    $ret .= "\n";
    return $ret;
}

sub add_scalerclkdiv
{
    my ($scaler) = @_;
    my ($clkdiv) = "_CLKDIV";
    my ($ret);
    $ret = "";
    for $i (0..31)
    {
	$ret .= "\#define   ${scaler}_$i${clkdiv}     $p\n"; $p++;
    }
    $ret .= "\n";
    return $ret;
}

sub add_scalercal
{
    my ($scaler) = @_;
    my ($cal) = "_CAL";
    my ($ret);
    $ret = "";
    for $i (0..31)
    {
	$ret .= "\#define   ${scaler}_$i${cal}     $p\n"; $p++;
    }
    $ret .= "\n";
    return $ret;
}

sub do_tirs
{
# TIRs

    $tiroff = $p;
    $out1 = &add_tir ("ITIRDATA");
    $out .= << "ENDTIRCOM";
// TIR data from various crates

\// TIRs start here
\#define   TIROFF     $tiroff
\ // number of TIRs defined below
\#define   TIRNUM     $ncrates

$out1
ENDTIRCOM

    $heloff = $p;
    $out1 = &add_tir ("IHELICITY");
    $out .= << "ENDHELCOM";
// Helicity info from various crates

\// Helicities start here
\#define   HELOFF     $heloff
\// number of HELs defined below
\#define   HELNUM     $ncrates

$out1
ENDHELCOM

    $timoff = $p;
    $out1 = &add_tir ("ITIMESLOT");
    $out .= << "ENDTIMCOM";
// Timeslot info from various crates

\// Timeslots start here
\#define   TIMOFF     $timoff
\// number of timeslots defined below
\#define   TIMNUM     $ncrates

$out1
ENDTIMCOM

    $paroff = $p;
    $out1 = &add_tir ("IPAIRSYNCH");
    $out .= << "ENDPARCOM";
// Pairsynch info from various crates

\// Pairsynchs start here
\#define   PAROFF     $paroff
\// number of pairsynchs defined below
\#define   PARNUM     $ncrates

$out1
ENDPARCOM
    $qudoff = $p;
    $out1 = &add_tir ("IMULTIPLETSYNCH");
    $out .= << "ENDMULCOM";
// Multipletsynch info from various crates

\// Multipletsynchs start here
\#define   MULOFF     $qudoff
\// number of multipletsynchs defined below
\#define   MULNUM     $ncrates

$out1
ENDMULCOM
}


sub do_daqflag
{
# DAQ flags

    $daqoff = $p;
    $out1 = "";
    $out1 .= &add_tir ("IDAQ1FLAG");
    $out1 .= "\n";
    $out1 .= &add_tir ("IDAQ2FLAG");
    $out1 .= "\n";
    $out1 .= &add_tir ("IDAQ3FLAG");
    $out1 .= "\n";
    $out1 .= &add_tir ("IDAQ4FLAG");
    $out1 .= "\n";
    $out1 .= &add_tir ("IDAQ5FLAG");
    $out1 .= "\n";
    $out1 .= &add_tir ("IDAQ6FLAG");
    $out1 .= "\n";
    $out1 .= &add_tir ("IDAQ7FLAG");
    $out1 .= "\n";
    $out1 .= &add_tir ("IDAQ8FLAG");
    $out1 .= "\n";
    $out1 .= &add_tir ("IDAQ9FLAG");
    $out1 .= "\n";
    $out1 .= &add_tir ("IDAQ10FLAG");
    $out1 .= "\n";
    $out1 .= &add_tir ("IDAQ11FLAG");
    $out1 .= "\n";
    $out1 .= &add_tir ("IDAQ12FLAG");

    $out .= << "ENDDAQCOM";
// DAQ Flag data from various crates
// (note: BMW data only comes from one crate)

\// DAQ flags start here
\#define   DAQOFF     $daqoff
\// number of DAQ flags defined below
\#define   DAQNUM     $ncrates


$out1
ENDDAQCOM
}


sub do_timeboards
{
# Timeboards

    $tbdoff = $p;
    $out1 = "";
    $out1 .= &add_tir ("ITIMEBOARD");
    $out1 .= "\n";
    $out1 .= &add_tir ("IRAMPDELAY");
    $out1 .= "\n";
    $out1 .= &add_tir ("IINTEGTIME");
    $out1 .= "\n";
    $out1 .= &add_tir ("IOVERSAMPLE");
    $out1 .= "\n";
    $out1 .= &add_tir ("IPRECDAC");
    $out1 .= "\n";
    $out1 .= &add_tir ("IPITADAC");

    $out .= << "ENDTBDCOM";
// Timeboard data from various crates

\// Timeboards start here
\#define   TBDOFF     $tbdoff
\// number of timeboards defined below
\#define   TBDNUM     $ncrates

$out1
ENDTBDCOM
}

sub add_tir
{
    my ($tir) = @_;
    my ($ret);
    $ret = "";
    $ret .= "\#define   ${tir}        $p\n"; $p++;
    $ret .= "\#define   ${tir}1       $p\n"; $p++;
    $ret .= "\#define   ${tir}2       $p\n"; $p++;
    $ret .= "\#define   ${tir}3       $p\n"; $p++;
    return $ret;
}

sub add_pitadac
{
    my ($pita) = @_;
    my ($ret);
    $ret = "\#define   ${pita}        $p\n"; $p++;
    return $ret;
}

sub do_lumis
{
# Lumi detectors

    $lmioff = $p;
    $lminum = scalar (@lumilist);
    $out1 = "";
    foreach $lumi (@lumilist)
    {
	$out1 .= &add_lumi ($lumi);
    }
    $out .= << "ENDLUMICOM";
// Lumi detectors

\// Lumis start here
\#define   LMIOFF     $lmioff
\// number of lumis defined below
\#define   LMINUM     $lminum

// R = raw data; "" = calibrated data

$out1
ENDLUMICOM

    $lmicorroff = $p;
    $out1 = "";
    foreach $lumi (@lumilist)
    {
	$out1 .= &add_lumicorr ($lumi);
    }
    $out .= << "ENDLUMICORRCOM";
// Lumi detectors (before pedestal subtraction)

\// Lumis (before peds)
\#define   LMICORROFF     $lmicorroff

// C = data before pedestal subtraction

$out1
ENDLUMICORRCOM
}

sub add_lumi
{
    my ($lumi) = @_;
    my ($ret);
    $ret = "";
    $ret .= "\#define   ${lumi}R     $p\n"; $p++;
    $ret .= "\#define   ${lumi}      $p\n"; $p++;
    $ret .= "\n";
    return $ret;
}

sub add_lumicorr
{
    my ($lumi) = @_;
    my ($ret);
    $ret = "";
    $ret .= "\#define   ${lumi}C     $p\n"; $p++;
    $ret .= "\n";
    return $ret;
}

sub do_v2fclocks
{
# V2F Clocks

    $v2fclkoff = $p;
    $v2fclknum = scalar (@v2fclocklist);
    $out1 = "";
    foreach $v2fclock (@v2fclocklist)
    {
	$out1 .= &add_v2fclock ($v2fclock);
    }
    $out .= << "ENDV2FCLOCKCOM";
// V2f_Clocks

\// V2F clocks start here
\#define   V2FCLKOFF     $v2fclkoff
\// number of V2F clocks defined below
\#define   V2FCLKNUM     $v2fclknum

$out1
ENDV2FCLOCKCOM
}

sub add_v2fclock
{
    my ($v2fclock) = @_;
    my ($ret);
    $ret = "\#define   ${v2fclock}      $p\n"; $p++;
    return $ret;
}


sub do_qpds
{
# Quad photodiodes

    $qpdoff = $p;
    $qpdnum = scalar (@qpdlist);
    $out1 = "";
    foreach $qpd (@qpdlist)
    {
	$out1 .= &add_qpd ($qpd);
    }
    
    $out .= << "ENDQPDCOM";
// Quad photodiodes

\// Quad photodiodes start here
\#define   QPDOFF     $qpdoff
\// number of quad photodiodes defined below
\#define   QPDNUM     $qpdnum

// PP, PM, MP, MM = diodes (x side, y side); X, Y = calibrated position;
// SUM = X, Y, and total diode sum

$out1
ENDQPDCOM

    $qpdcorroff = $p;
    $out1 = "";
    foreach $qpd (@qpdlist)
    {
	$out1 .= &add_qpdcorr ($qpd);
    }
    
    $out .= << "ENDQPDCORRCOM";
// Quad photodiodes (before pedestal subtraction)

\// Quad photodiodes (before peds)
\#define   QPDCORROFF     $qpdcorroff

// PPC, PMC, MPC, MMC = diodes (x side, y side) before pedestal subtraction

$out1
ENDQPDCORRCOM
}

sub add_qpd
{
    my ($qpd) = @_;
    my ($ret);
    $ret = "";
    $ret .= "\#define   ${qpd}PP   $p\n"; $p++;
    $ret .= "\#define   ${qpd}PM   $p\n"; $p++;
    $ret .= "\#define   ${qpd}MP   $p\n"; $p++;
    $ret .= "\#define   ${qpd}MM   $p\n"; $p++;
    $ret .= "\#define   ${qpd}X    $p\n"; $p++;
    $ret .= "\#define   ${qpd}Y    $p\n"; $p++;
    $ret .= "\#define   ${qpd}SUM  $p\n"; $p++;
    $ret .= "\n";
    return $ret;
}

sub add_qpdcorr
{
    my ($qpd) = @_;
    my ($ret);
    $ret = "";
    $ret .= "\#define   ${qpd}PPC   $p\n"; $p++;
    $ret .= "\#define   ${qpd}PMC   $p\n"; $p++;
    $ret .= "\#define   ${qpd}MPC   $p\n"; $p++;
    $ret .= "\#define   ${qpd}MMC   $p\n"; $p++;
    $ret .= "\n";
    return $ret;
}

# --------------LINA -----------------------
sub do_linas
{
# Linear Arrays

    $linaoff = $p;
    $linanum = scalar (@linalist);
    $out1 = "";
    foreach $lina (@linalist)
    {
	$out1 .= &add_lina ($lina);
    }
    
    $out .= << "ENDLINACOM";
// Linear Arrays

\// Linear Array starts here
\#define   LINAOFF     $linaoff
\// number of quad photodiodes defined below
\#define   LINANUM     $linanum

// 1-8 diode pads.  
// Sum = 0th moment
//   x = 1st moment 
// rms = 2nd moment

$out1
ENDLINACOM

}

sub add_lina
{
    my ($lina) = @_;
    my ($ret);
    $ret = "";
    $ret .= "\#define   ${lina}_1R   $p\n"; $p++;
    $ret .= "\#define   ${lina}_2R   $p\n"; $p++;
    $ret .= "\#define   ${lina}_3R   $p\n"; $p++;
    $ret .= "\#define   ${lina}_4R   $p\n"; $p++;
    $ret .= "\#define   ${lina}_5R   $p\n"; $p++;
    $ret .= "\#define   ${lina}_6R   $p\n"; $p++;
    $ret .= "\#define   ${lina}_7R   $p\n"; $p++;
    $ret .= "\#define   ${lina}_8R   $p\n"; $p++;
    $ret .= "\#define   ${lina}_1   $p\n"; $p++;
    $ret .= "\#define   ${lina}_2   $p\n"; $p++;
    $ret .= "\#define   ${lina}_3   $p\n"; $p++;
    $ret .= "\#define   ${lina}_4   $p\n"; $p++;
    $ret .= "\#define   ${lina}_5   $p\n"; $p++;
    $ret .= "\#define   ${lina}_6   $p\n"; $p++;
    $ret .= "\#define   ${lina}_7   $p\n"; $p++;
    $ret .= "\#define   ${lina}_8   $p\n"; $p++;
    $ret .= "\#define   ${lina}SUM  $p\n"; $p++;
    $ret .= "\#define   ${lina}X    $p\n"; $p++;
    $ret .= "\#define   ${lina}RMS  $p\n"; $p++;
    $ret .= "\#define   ${lina}XG    $p\n"; $p++;
    $ret .= "\#define   ${lina}RMSG  $p\n"; $p++;
    $ret .= "\n";
    return $ret;
}

#----------End LINA---------------

sub do_bmwwords
{
# Beam modulation information

    $bmwoff = $p;
    $bmwnum = scalar (@bmwlist);
    $out1 = "";
    foreach $bmwword (@bmwlist)
    {
	$out1 .= &add_bmwword ($bmwword);
    }
    $out .= << "ENDBMWWORDCOM";
// bmw words

\// bmw words start here
\#define   BMWOFF     $bmwoff
\// number of bmw words defined below
\#define   BMWNUM     $bmwnum

$out1
ENDBMWWORDCOM

    $bmwcoiloff = $p;
    $bmwcoilnum = scalar (@bmwcoillist);
    $out1 = "";
    foreach $bmwcoilword (@bmwcoillist)
    {
	$out1 .= &add_bmwword ($bmwcoilword);
    }
    $out .= << "ENDBMWCOILWORDCOM";
// bmwcoil words

\// bmwcoil words start here
\#define   BMWCOILOFF     $bmwcoiloff
\// number of bmwcoil words defined below
\#define   BMWCOILNUM     $bmwcoilnum

$out1
ENDBMWCOILWORDCOM

}

sub add_bmwword
{
    my ($bmwword) = @_;
    my ($ret);
    $ret = "\#define   ${bmwword}      $p\n"; $p++;
    return $ret;
}

sub do_scanflags
{
# Scan flags

    $scanfloff = $p;
    $scanflnum = scalar (@scanflist);
    $out1 = "";
    foreach $scanfl (@scanflist)
    {
	$out1 .= &add_scanflag ($scanfl);
    }
    $out .= << "ENDSCANFLCOM";
// scan flags

\// scanflags start here
\#define   SCANFLAGOFF     $scanfloff
\// number of scanflags defined below
\#define   SCANFLAGNUM     $scanflnum

$out1
ENDSCANFLCOM
}

sub add_scanflag
{
    my ($scanfl) = @_;
    my ($ret);
    $ret = "\#define   ${scanfl}      $p\n"; $p++;
    return $ret;
}

sub do_syncwords
{
# Crate synchronization information

    $syncoff = $p;
    $syncnum = scalar (@synclist);
    $out1 = "";
    foreach $syncword (@synclist)
    {
	$out1 .= &add_syncword ($syncword);
    }
    $out .= << "ENDSYNCWORDCOM";
// sync words

\// sync words start here
\#define   SYNCOFF     $syncoff
\// number of sync words defined below
\#define   SYNCNUM     $syncnum

$out1
ENDSYNCWORDCOM
}

sub add_syncword
{
    my ($syncword) = @_;
    my ($ret);
    $ret = "\#define   ${syncword}      $p\n"; $p++;
    return $ret;
}

