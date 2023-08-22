#include <TFile.h>
#include <TTree.h>
#include <string.h>
#include <TMath.h>

bool producedInPhantom(double x, double y, double z);

void edge_hit_xy(std::string path, std::string particle)
{

    // constants
    double energy_min = 100000; // eV

    TFile *file = new TFile(path.c_str(), "read");
    TTree *hits = (TTree *)file->Get("hits");

    auto h1_2d = new TH2D("edges_det_0", "edges detector 0; x [mm]; y [mm]", 1000, -35, 35, 1000, -35, 35);
    auto h2_2d = new TH2D("edges_det_1", "edges detector 1; x [mm]; y [mm]", 1000, -35, 35, 1000, -35, 35);
    auto h3_2d = new TH2D("edges_det_2", "edges detector 2; x [mm]; y [mm]", 1000, -35, 35, 1000, -35, 35);
    auto h4_2d = new TH2D("edges_det_3", "edges detector 3; x [mm]; y [mm]", 1000, -35, 35, 1000, -35, 35);

    auto prod_h1_1i = new TH1I("produced", "origin detector 0", 2, 0, 2);
    auto prod_h2_1i = new TH1I("produced", "origin detector 1", 2, 0, 2);
    auto prod_h3_1i = new TH1I("produced", "origin detector 2", 2, 0, 2);
    auto prod_h4_1i = new TH1I("produced", "origin detector 3", 2, 0, 2);


    char particle_name[128];
    Int_t event_number;
    Int_t det_id;
    Int_t strip_id;
    Double_t edep, energy;
    Double_t hits_x, hits_y, hits_z;
    Double_t produced_x, produced_y, produced_z;

    hits->SetBranchAddress("hits_particle_name_", &particle_name);
    hits->SetBranchAddress("hits_event_number_", &event_number);
    hits->SetBranchAddress("hits_detId_", &det_id);
    hits->SetBranchAddress("hits_stripId_", &strip_id);
    hits->SetBranchAddress("hits_edep_", &edep);
    hits->SetBranchAddress("hits_x_", &hits_x);
    hits->SetBranchAddress("hits_y_", &hits_y);
    hits->SetBranchAddress("hits_z_", &hits_z);
    hits->SetBranchAddress("hits_energy_", &energy);
    hits->SetBranchAddress("produced_x_", &produced_x);
    hits->SetBranchAddress("produced_y_", &produced_y);
    hits->SetBranchAddress("produced_z_", &produced_z);



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
                x1.push_back(x*TMath::Cos(TMath::Pi()/4) - z* TMath::Sin(TMath::Pi()/4));
                y1.push_back(y);
            } else if((det_id==2 && back_size_det_2 == 0) || (det_id==3 && front_size_det_2 == 0)){
                x2.push_back(x*TMath::Cos(TMath::Pi()/4) - z* TMath::Sin(TMath::Pi()/4));
                y2.push_back(y);
            } else if((det_id==4 && back_size_det_3 == 0) || (det_id==5 && front_size_det_3 == 0)){
                x3.push_back(x*TMath::Cos(-TMath::Pi()/4) - z* TMath::Sin(-TMath::Pi()/4));
                y3.push_back(y);
            }else if((det_id==6 && back_size_det_4 == 0) || (det_id==7 && front_size_det_4 == 0)){
                x4.push_back(x*TMath::Cos(-TMath::Pi()/4) - z* TMath::Sin(-TMath::Pi()/4));
                y4.push_back(y);
            }

        }

        // check if "one-hitter"
        if ((front_size_det_1 > 0 && back_size_det_1 == 0) || (back_size_det_1 > 0 && front_size_det_1 == 0))
        {
            for(int i = 0; i< x1.size(); i++){
                h1_2d->Fill(x1[i], y1[i]);
            }
            if(producedInPhantom(produced_x, produced_y, produced_z)){
                prod_h1_1i->Fill("origin in Phantom");
            } else {
                prod_h1_1i->Fill("origin elsewhere");
            }
        }

        if ((front_size_det_2 > 0 && back_size_det_2 == 0) || (back_size_det_2 > 0 && front_size_det_2 == 0))
        {
            for(int i = 0; i< x2.size(); i++){
                h2_2d->Fill(x2[i], y2[i]);
            }
            if(producedInPhantom(produced_x, produced_y, produced_z)){
                prod_h2_1i->Fill("origin in Phantom");
            } else {
                prod_h2_1i->Fill("origin elsewhere");
            }
        }

        if ((front_size_det_3 > 0 && back_size_det_3 == 0) || (back_size_det_3 > 0 && front_size_det_3 == 0))
        {
            for(int i = 0; i< x3.size(); i++){
                h3_2d->Fill(x3[i], y3[i]);
            }

            if(producedInPhantom(produced_x, produced_y, produced_z)){
                prod_h3_1i->Fill("origin in Phantom");
            } else {
                prod_h3_1i->Fill("origin elsewhere");
            }
        }

        if ((front_size_det_4 > 0 && back_size_det_4 == 0) || (back_size_det_4 > 0 && front_size_det_4 == 0))
        {
            for(int i = 0; i< x4.size(); i++){
                h4_2d->Fill(x4[i], y4[i]);
            }

            if(producedInPhantom(produced_x, produced_y, produced_z)){
                prod_h4_1i->Fill("origin in Phantom");
            } else {
                prod_h4_1i->Fill("origin elsewhere");
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
    h3_2d->Draw();
    c2->cd(2);
    h4_2d->Draw();
    c2->cd(3);
    h1_2d->Draw();
    c2->cd(4);
    h2_2d->Draw();

    auto c1 = new TCanvas((std::string("origin_XY_particle==") + particle).c_str(), (std::string("origin_XY_emin=100keV_particle==") + particle).c_str());
    c1->Divide(2, 2);
    c1->cd(1);
    prod_h3_1i->Draw();
    c1->cd(2);
    prod_h4_1i->Draw();
    c1->cd(3);
    prod_h1_1i->Draw();
    c1->cd(4);
    prod_h2_1i->Draw();
}

//return if given coordinates are in phantom
bool inPproducedInPhantom(double x, double y, double z){
    double Phantom_X = 0.0;
    double Phantom_Y = 0.0;
    double Phantom_Z = 0.0;
    double Phantom_DEPTH = 200; //mm
    double Phantom_RADIUS = 100.; //mm

    if((std::pow((x-Phantom_X), 2) + std::pow((z-Phantom_Z),2) <= std::pow(Phantom_RADIUS, 2)) 
        && z <= Phantom_DEPTH/2 && z>= -Phantom_DEPTH/2 ){
            //position is within the Phantom
            return true;
        } esle {return false;}

}