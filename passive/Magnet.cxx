/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             *
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/

// -------------------------------------------------------------------------
// -----                    Magnet  file                               -----
// -----                Created 26/03/14  by M. Al-Turany              -----
// -------------------------------------------------------------------------

#include "Magnet.h"
#include "TGeoBBox.h"            // for TGeoBBox
#include "TGeoCompositeShape.h"  // for TGeoCompositeShape
#include "TGeoManager.h"         // for TGeoManager, gGeoManager
#include "TGeoMaterial.h"        // for TGeoMaterial
#include "TGeoMatrix.h"          // for TGeoTranslation, TGeoCombiTrans, etc
#include "TGeoMedium.h"          // for TGeoMedium
#include "TGeoTube.h"            // for TGeoTubeSeg
#include "TGeoVolume.h"          // for TGeoVolume
#include <stddef.h>                     // for NULL
#include <iostream>                     // for operator<<, basic_ostream, etc

using namespace AliceO2::Passive;


Magnet::~Magnet()
{
}
Magnet::Magnet()
  : FairModule("Magnet", "")
{
}

Magnet::Magnet(const char* name, const char* Title)
  : FairModule(name ,Title)
{
}

Magnet::Magnet(const Magnet& rhs)
  : FairModule(rhs)
{
}

Magnet& Magnet::operator=(const Magnet& rhs)
{
  // self assignment
  if (this == &rhs) return *this;

  // base class assignment
  FairModule::operator=(rhs);

  return *this;
}

void Magnet::ConstructGeometry()
{
   
    TGeoVolume *top=gGeoManager->GetTopVolume();
    
    // define some materials
    TGeoMaterial *matFe     = new TGeoMaterial("Fe", 55.84, 26, 7.9);

    // define some media
    TGeoMedium *Fe     = new TGeoMedium("Fe", 3, matFe);
    

    // magnet yoke
    TGeoBBox *magyoke1 = new TGeoBBox("magyoke1", 350, 350, 125);
    TGeoBBox *magyoke2 = new TGeoBBox("magyoke2", 250, 250, 126);
    
    TGeoCompositeShape *magyokec = new TGeoCompositeShape("magyokec", "magyoke1-magyoke2");
    TGeoVolume *magyoke = new TGeoVolume("magyoke", magyokec, Fe);
    magyoke->SetLineColor(kBlue);
    //magyoke->SetTransparency(50);
    top->AddNode(magyoke, 1, new TGeoTranslation(0, 0, 0));
    
    // magnet
    TGeoTubeSeg *magnet1a = new TGeoTubeSeg("magnet1a", 250, 300, 35, 45, 135);
    TGeoTubeSeg *magnet1b = new TGeoTubeSeg("magnet1b", 250, 300, 35, 45, 135);
    TGeoTubeSeg *magnet1c = new TGeoTubeSeg("magnet1c", 250, 270, 125, 45, 60);
    TGeoTubeSeg *magnet1d = new TGeoTubeSeg("magnet1d", 250, 270, 125, 120, 135);
    
    // magnet composite shape matrices
    TGeoTranslation *m1 = new TGeoTranslation(0, 0, 160);
    m1->SetName("m1");
    m1->RegisterYourself();
    TGeoTranslation *m2 = new TGeoTranslation(0, 0, -160);
    m2->SetName("m2");
    m2->RegisterYourself();
    
    TGeoCompositeShape *magcomp1 = new TGeoCompositeShape("magcomp1", "magnet1a:m1+magnet1b:m2+magnet1c+magnet1d");
    TGeoVolume *magnet1 = new TGeoVolume("magnet1", magcomp1, Fe);
    magnet1->SetLineColor(kYellow);
    top->AddNode(magnet1, 1, new TGeoTranslation(0, 0, 0));
    
    TGeoRotation m3;
    m3.SetAngles(180, 0, 0);
    TGeoTranslation m4(0, 0, 0);
    TGeoCombiTrans m5(m4, m3);
    TGeoHMatrix *m6 = new TGeoHMatrix(m5);
    top->AddNode(magnet1, 2, m6);
    
    
}

FairModule* Magnet::CloneModule() const
{
  return new Magnet(*this);
}

ClassImp(AliceO2::Passive::Magnet)














