// File: reformatData.C
// Purpose: Reformat CMS and ATLAS HEPDATA to have same format,
//          1) TGraph2DErrors with stat+syst
//          2) TGraph2DErrors with stat only
// Next:
//          3) TGraph2DErrors with syst only
//          4) TH1D with stat only
//          5) Folder with uncertainty sources up and down
//

#include "TFile.h"
#include "TGraphAsymmErrors.h"
#include "TH1D.h"

#include <fstream>
#include <iostream>
#include <string>

// Discussions in LHC-EW WG: Jets and EW bosons
// 13 June, 2018: https://indico.cern.ch/event/733183/
// => Bogdan on inclusive jet uncertainties, Mikko on 7 TeV combination

void reformat(TFile *f, char *path, char *t1, char *t2, char *folder, int pos) {
  
  TFile *f7 = new TFile(path,"READ");
  assert(f7 && !f7->IsZombie());

  f->cd();

  //Folder for systematic uncertainties
  TDirectory *sys = f->mkdir(folder);

  //Title of files
  string title;

  for (int i = 0; i != 12; ++i) {

    if (i<6) title = Form(t1,5*i,5*(i+1));
    else     title = Form(t2,5*(i-6),5*(i-6+1));

    f7->cd();
    f7->cd(Form("Table %d",i+1));

    //Name of Graph/Histogram
    string name;
    //Index of Graph/Histogram (1,2 or 3)
    char index;
    //Start part of name of systematic uncertainties (r%02d_y%02d-%02d)
    string start_sys;
    //End part of name of systematic uncertainties (1, 1plus, 1minus etc)
    string end_sys;

    //Iterate over entries in Table of f7
    TIter next (gDirectory->GetListOfKeys());
    TKey *key;

    while ((key = (TKey*)next())) {

      //If Graph
      if (!strcmp(key->GetClassName(),"TGraphAsymmErrors")) {

	name = key->GetName();
	index = name[9];
	
	TGraphAsymmErrors *g = (TGraphAsymmErrors*)key->ReadObj();
	assert(g);

	g->SetName(Form("%s_%c",title.c_str(),index));

	TH1D *hs = (TH1D*)f7->Get(Form("Table %d/Hist1D_y1_e1",i+1));
	assert(hs);

	TGraphAsymmErrors *gs = (TGraphAsymmErrors*)g->Clone(Form("%s_stat",g->GetName()));
	for (int i = 0; i != gs->GetN(); ++i) {
	  double x = gs->GetX()[i];
	  int ix = hs->FindBin(x);
	  double ex = 0.5*hs->GetBinWidth(ix);
	  double ey = hs->GetBinError(ix);
	  gs->SetPointError(i, ex,ex, ey,ey);
	}

	//Write it into main folder
	f->cd();
	g->Write();
	gs->Write();
	
      }
      

      //If Histogram
      if (!strcmp(key->GetClassName(),"TH1F")) {

	name = key->GetName();
	index = name[8];
	start_sys = title.substr(pos);

	TH1F *hs = (TH1F*)key->ReadObj();
	assert(hs);

	//Just a way to get around that some histogram names have lengths less than 11 bytes
	if (name.length() == 9) {

	  hs->SetName(Form("%s_%c_sys",start_sys.c_str(),index));
	  
	} else {

	  end_sys = name.substr(11);

	  hs->SetName(Form("%s_%c_sys_%s",start_sys.c_str(),index,end_sys.c_str()));

	}

	//Write it into subdirectory
	f->cd();
	gDirectory->cd(folder);
	hs->Write();


     }

   }


  }

}

void reformatData_version2() {

  // Output file
  TFile *f = new TFile("lhcdata.root","RECREATE");

  // ATLAS 7 TeV, R=0.4 and R=0.6, 4.5/fb
  // https://www.hepdata.net/download/submission/ins1325553/1/root
  char path_atlas[] = "atlas/HEPData-ins1325553-v1-root.root";  
  char folder_atlas[] = "atlas07_sys";
  char t1_atlas[] = "atlas07_r04_y%02d-%02d";
  char t2_atlas[] = "atlas07_r06_y%02d-%02d";   
  int pos_atlas = 8;

  // CMS 7 TeV, R=0.5 and R=0.7, 5.0/fb
  // https://www.hepdata.net/download/submission/ins1298810/1/root
  // (+uncertainty tables from Resources:
  // https://www.hepdata.net/record/resource/63665?view=true )  
  char path_cms[] = "cms/HEPData-ins1298810-v1-root.root";  
  char folder_cms[] = "cms07_sys";
  char t1_cms[] = "cms07_r05_y%02d-%02d";
  char t2_cms[] = "cms07_r07_y%02d-%02d"; 
  char pos_cms = 6;

  reformat(f, path_atlas, t1_atlas, t2_atlas, folder_atlas, pos_atlas); 

  reformat(f, path_cms, t1_cms, t2_cms, folder_cms, pos_cms);
  }

  /*

  // CMS 8 TeV, R=0.5 and R=0.7, 19.7/fb
  // arXiv:1609.05331
  // https://hepdata.net/download/submission/ins1487277/1/root
  // (+uncertainty tables from Resources / xFitter Analysis:
  // http://www.hepforge.org/archive/xfitter/1609.05331.tar.gz )
  // (duh, only R=0.7 done :(, cannot interpolate properly)
  TFile *fc8 = new TFile("cms/HEPData-ins1487277-v1-root.root","READ");
  assert(fc8 && !fc8->IsZombie());

  for (int i = 0; i != 12; ++i) {
      
    TGraphAsymmErrors *g = (TGraphAsymmErrors*)fc8->Get(Form("Table %d/Graph1D_y1",i+1));
    assert(g);

    if (i<6) g->SetName(Form("cms07_r05_y%02d-%02d",5*i,5*(i+1)));
    else     g->SetName(Form("cms07_r07_y%02d-%02d",5*(i-6),5*(i-6+1)));
    
    TH1D *hs = (TH1D*)fc8->Get(Form("Table %d/Hist1D_y1_e1",i+1));
    assert(hs);

    TGraphAsymmErrors *gs = (TGraphAsymmErrors*)g->Clone(Form("%s_stat",g->GetName()));
    for (int i = 0; i != gs->GetN(); ++i) {
      double x = gs->GetX()[i];
      int ix = hs->FindBin(x);
      double ex = 0.5*hs->GetBinWidth(ix);
      double ey = hs->GetBinError(ix);
      gs->SetPointError(i, ex,ex, ey,ey);
    }

    f->cd();
    g->Write();
    gs->Write();
  }
  
  */

