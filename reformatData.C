// File: reformatData.C
// Purpose: Reformat CMS and ATLAS HEPDATA to have same format,
//          1) TGraph2DErrors with stat+syst
//          2) TGraph2DErrors with stat only
// Next:
//          3) TGraph2DErrors with syst only
//          4) TH1D with stat only
//          5) Folder with uncertainty sources up and down
//
//
#include "TFile.h"
#include "TGraphAsymmErrors.h"
#include "TH1D.h"

// Discussions in LHC-EW WG: Jets and EW bosons
// 13 June, 2018: https://indico.cern.ch/event/733183/
// => Bogdan on inclusive jet uncertainties, Mikko on 7 TeV combination

void reformatData() {

  // Output file
  TFile *f = new TFile("lhcdata.root","RECREATE");

  // ATLAS 7 TeV, R=0.4 and R=0.6, 4.5/fb
  // https://www.hepdata.net/download/submission/ins1325553/1/root
  TFile *fa7 = new TFile("atlas/HEPData-ins1325553-v1-root.root","READ");
  assert(fa7 && !fa7->IsZombie());

  f->cd();

  //Folder for systematic uncertainties
  TDirectory *sda7_stat = f->mkdir("atlas07_sys");

  for (int i = 0; i != 12; ++i) {

    string title;
    if (i<6) title = Form("atlas07_r04_y%02d-%02d",5*i,5*(i+1));
    else     title = Form("atlas07_r06_y%02d-%02d",5*(i-6),5*(i-6+1));

    fa7->cd();
    fa7->cd(Form("Table %d",i+1));

    //Name of Graph/Histogram
    string name;
    //Char of index of Graoṕh/Histogram
    char index;
    //Start part of name of systematic uncertainties
    string start_sys;
    //End part of name of systematic uncertainties
    string end_sys;

    //Iterate over entries in Table of fa7
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

	TH1D *hs = (TH1D*)fa7->Get(Form("Table %d/Hist1D_y1_e1",i+1));
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
	start_sys = title.substr(8);

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
	gDirectory->cd("atlas07_sys");
	hs->Write();


      }

   }


  }
 
  // CMS 7 TeV, R=0.5 and R=0.7, 5.0/fb
  // https://www.hepdata.net/download/submission/ins1298810/1/root
  // (+uncertainty tables from Resources:
  // https://www.hepdata.net/record/resource/63665?view=true )
  TFile *fc7 = new TFile("cms/HEPData-ins1298810-v1-root.root","READ");
  assert(fc7 && !fc7->IsZombie());

  f->cd();
  
  //Folder for systematic uncertainties
  TDirectory *sdc7_sys = f->mkdir("cms07_sys");

  for (int i = 0; i != 12; ++i) {

    string title;
    if (i<6) title = Form("cms07_r05_y%02d-%02d",5*i,5*(i+1));
    else     title = Form("cms07_r07_y%02d-%02d",5*(i-6),5*(i-6+1));

    fc7->cd();
    fc7->cd(Form("Table %d",i+1));

    //Name of Graph/Histogram
    string name;
    //Char of index of Graoṕh/Histogram
    char index;
    //Start part of name of systematic uncertainties
    string start_sys;
    //End part of name of systematic uncertainties
    string end_sys;

    //Iterate over entries in Table in fc7
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

	TH1D *hs = (TH1D*)fc7->Get(Form("Table %d/Hist1D_y1_e1",i+1));
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
	start_sys = title.substr(8);

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
	gDirectory->cd("cms07_sys");
	hs->Write();

      }

   }

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

}
