#include <TFile.h>
#include <TTree.h>
#include <string.h>
#include <TMath.h>
#include <TGraph.h>

bool producedInPhantom(double x, double y, double z);

void edge_hit_xy(std::string path, std::string particle)
{

    // constants
    double energy_min = 100000; // eV

    TFile *file = new TFile(path.c_str(), "read");
    TTree *hits = (TTree *)file->Get("hits");

    TGraph* gr1;
    TGraph* gr2;
    TGraph* gr3;
    TGraph*  gr4;
    gr1 = new TGraph(); 
    gr1->SetTitle("detector 0; x [mm]; y [mm]");
    gr2 = new TGraph(); 
    gr2->SetTitle("detector 1; x [mm]; y [mm]");
    gr3 = new TGraph(); 
    gr3->SetTitle("detector 2; x [mm]; y [mm]");
    gr4 = new TGraph(); 
    gr4->SetTitle("detector 3; x [mm]; y [mm]");


    char particle_name[128];
    Int_t event_number;
    Int_t det_id;
    Int_t strip_id;
    Double_t edep, energy;
    Double_t hits_x, hits_y, hits_z;
    Double_t produced_x, produced_y, produced_z;

    hits->SetBranchAddress("name", &particle_name);
    hits->SetBranchAddress("event", &event_number);
    hits->SetBranchAddress("Det_ID", &det_id);
    hits->SetBranchAddress("Strip_ID", &strip_id);
    hits->SetBranchAddress("edep", &edep);
    hits->SetBranchAddress("Hit_x", &hits_x);
    hits->SetBranchAddress("Hit_y", &hits_y);
    hits->SetBranchAddress("Hit_z", &hits_z);
    hits->SetBranchAddress("Hit_energy", &energy);
    hits->SetBranchAddress("Produced_x", &produced_x);
    hits->SetBranchAddress("Produced_y", &produced_y);
    hits->SetBranchAddress("Produced_z", &produced_z);



    // preload stuff to speed things up
    hits->LoadBaskets();

    int i = 0;
    int size = hits->GetEntries();

    int event_size_det_1 = 0;
    int event_size_det_2 = 0;
    int event_size_det_3 = 0;
    int event_size_det_4 = 0;

    int front_size_det_1 = 0;
    int front_size_det_2 = 0;
    int front_size_det_3 = 0;
    int front_size_det_4 = 0;

    int back_size_det_1 = 0;
    int back_size_det_2 = 0;
    int back_size_det_3 = 0;
    int back_size_det_4 = 0;
    int event_size = 0;

    while (i < size) // loop through all events
    {

        event_size_det_1 = 0;
        event_size_det_2 = 0;
        event_size_det_3 = 0;
        event_size_det_4 = 0;

        front_size_det_1 = 0;
        front_size_det_2 = 0;
        front_size_det_3 = 0;
        front_size_det_4 = 0;

        back_size_det_1 = 0;
        back_size_det_2 = 0;
        back_size_det_3 = 0;
        back_size_det_4 = 0;

        hits->GetEntry(i);
        int currEvent = event_number;
        bool sameEvent = true;
        std::vector<double> x1, x2, x3, x4;
        std::vector<double> y1, y2, y3, y4;

        while (sameEvent && i < size) // loop through one full event, increase i while doing so
        {
            hits->GetEntry(i);

            if (event_number == currEvent)
            {
                // we are still looking at the right event

                if (edep > energy_min && std::string(particle_name).compare(particle) == 0) // only count event if energy is deposited  && std::string(particle_name).compare("e+")==0 
                {
                    if (det_id == 0)
                    {
                        front_size_det_1 += 1;
                    }
                    else if (det_id == 1)
                    {
                        back_size_det_1 += 1;
                    }
                    else if (det_id == 2)
                    {
                        front_size_det_2 += 1;
                    }
                    else if (det_id == 3)
                    {
                        back_size_det_2 += 1;
                    }
                    else if (det_id == 4)
                    {
                        front_size_det_3 += 1;
                    }
                    else if (det_id == 5)
                    {
                        back_size_det_3 += 1;
                    }
                    else if (det_id == 6)
                    {
                        front_size_det_4 += 1;
                    }
                    else if (det_id == 7)
                    {
                        back_size_det_4 += 1;
                    }
                    i += 1;
                }
                else
                {
                    i += 1;
                }
            }
            else
            {
                sameEvent = false;
            }

            if((det_id==0 && back_size_det_1 == 0) || (det_id==1 && front_size_det_1 == 0)){
                x1.push_back(hits_x*TMath::Cos(TMath::Pi()/4) - hits_z* TMath::Sin(TMath::Pi()/4));
                y1.push_back(hits_y);
            } else if((det_id==2 && back_size_det_2 == 0) || (det_id==3 && front_size_det_2 == 0)){
                x2.push_back(hits_x*TMath::Cos(TMath::Pi()/4) - hits_z* TMath::Sin(TMath::Pi()/4));
                y2.push_back(hits_y);
            } else if((det_id==4 && back_size_det_3 == 0) || (det_id==5 && front_size_det_3 == 0)){
                x3.push_back(hits_x*TMath::Cos(-TMath::Pi()/4) - hits_z* TMath::Sin(-TMath::Pi()/4));
                y3.push_back(hits_y);
            }else if((det_id==6 && back_size_det_4 == 0) || (det_id==7 && front_size_det_4 == 0)){
                x4.push_back(hits_x*TMath::Cos(-TMath::Pi()/4) - hits_z* TMath::Sin(-TMath::Pi()/4));
                y4.push_back(hits_y);
            }

        }

        // check if "one-hitter"
        if ((front_size_det_1 > 0 && back_size_det_1 == 0) || (back_size_det_1 > 0 && front_size_det_1 == 0))
        {
            for(int i = 0; i< x1.size(); i++){
                gr1->AddPoint(x1[i], y1[i]);
            }
            
        }

        if ((front_size_det_2 > 0 && back_size_det_2 == 0) || (back_size_det_2 > 0 && front_size_det_2 == 0))
        {
            for(int i = 0; i< x2.size(); i++){
                gr2->AddPoint(x2[i], y2[i]);
            }
            
        }

        if ((front_size_det_3 > 0 && back_size_det_3 == 0) || (back_size_det_3 > 0 && front_size_det_3 == 0))
        {
            for(int i = 0; i< x3.size(); i++){
                gr3->AddPoint(x3[i], y3[i]);
            }

            
        }

        if ((front_size_det_4 > 0 && back_size_det_4 == 0) || (back_size_det_4 > 0 && front_size_det_4 == 0))
        {
            for(int i = 0; i< x4.size(); i++){
                gr4->AddPoint(x4[i], y4[i]);
            }

            
        }
        x1.resize(0);
        y1.resize(0);
        x2.resize(0);
        y2.resize(0);
        x3.resize(0);
        y3.resize(0);
        x4.resize(0);
        y4.resize(0);
    }

    auto c2 = new TCanvas((std::string("edge_hitsXY_particle==") + particle).c_str(), (std::string("edge hits (XY)for ") + particle + std::string(" (e_min= 100keV)")).c_str());
    c2->SetCanvasSize(1500,1500);
    c2->Divide(2, 2);
    c2->cd(1);
    gr3->Draw("AP");
    c2->cd(2);
    gr4->Draw("AP");
    c2->cd(3);
    gr1->Draw("AP");
    c2->cd(4);
    gr2->Draw("AP");
}

//return if given coordinates are in phantom
bool producedInPhantom(double x, double y, double z){
    double Phantom_X = 0.0;
    double Phantom_Y = 0.0;
    double Phantom_Z = 0.0;
    double Phantom_DEPTH = 200; //mm
    double Phantom_RADIUS = 100.; //mm

    if((std::pow((x-Phantom_X), 2) + std::pow((z-Phantom_Z),2) <= std::pow(Phantom_RADIUS, 2)) 
        && z <= Phantom_DEPTH/2 && z>= -Phantom_DEPTH/2 ){
            //position is within the Phantom
            return true;
        } else {return false;}

}