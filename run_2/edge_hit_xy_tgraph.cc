#include <TFile.h>
#include <TTree.h>
#include <string.h>
#include <TMath.h>
#include <TGraph.h>

bool producedInPhantom(double x, double y, double z);

void edge_hit_xy_tgraph(std::string path, std::string particle)
{

    // constants
    double energy_min = 100000; // eV

    TFile *file = new TFile(path.c_str(), "read");
    TTree *hits = (TTree *)file->Get("hits");

    std::vector<double> x1_phantom;
    std::vector<double> y1_phantom;
    std::vector<double> x1_outside;
    std::vector<double> y1_outside;

    std::vector<double> x2_phantom;
    std::vector<double> y2_phantom;
    std::vector<double> x2_outside;
    std::vector<double> y2_outside;

    std::vector<double> x3_phantom;
    std::vector<double> y3_phantom;
    std::vector<double> x3_outside;
    std::vector<double> y3_outside;

    std::vector<double> x4_phantom;
    std::vector<double> y4_phantom;
    std::vector<double> x4_outside;
    std::vector<double> y4_outside;

    // auto h1_2d = new TH2D("edges_det_0", "edges detector 0; x [mm]; y [mm]", 1000, -35, 35, 1000, -35, 35);
    // auto h2_2d = new TH2D("edges_det_1", "edges detector 1; x [mm]; y [mm]", 1000, -35, 35, 1000, -35, 35);
    // auto h3_2d = new TH2D("edges_det_2", "edges detector 2; x [mm]; y [mm]", 1000, -35, 35, 1000, -35, 35);
    // auto h4_2d = new TH2D("edges_det_3", "edges detector 3; x [mm]; y [mm]", 1000, -35, 35, 1000, -35, 35);

    // auto prod_h1_1i = new TH1I("produced", "origin detector 0; ; #", 2, 0, 2);
    // auto prod_h2_1i = new TH1I("produced", "origin detector 1; ; #", 2, 0, 2);
    // auto prod_h3_1i = new TH1I("produced", "origin detector 2; ; #", 2, 0, 2);
    // auto prod_h4_1i = new TH1I("produced", "origin detector 3; ; #", 2, 0, 2);

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
        std::vector<double> prod_x1, prod_x2, prod_x3, prod_x4;
        std::vector<double> prod_y1, prod_y2, prod_y3, prod_y4;
        std::vector<double> prod_z1, prod_z2, prod_z3, prod_z4;

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

            if ((det_id == 0 && back_size_det_1 == 0) || (det_id == 1 && front_size_det_1 == 0))
            {
                x1.push_back(hits_x * TMath::Cos(TMath::Pi() / 4) - hits_z * TMath::Sin(TMath::Pi() / 4));
                y1.push_back(hits_y);
                prod_x1.push_back(produced_x);
                prod_y1.push_back(produced_y);
                prod_z1.push_back(produced_z);
            }
            else if ((det_id == 2 && back_size_det_2 == 0) || (det_id == 3 && front_size_det_2 == 0))
            {
                x2.push_back(hits_x * TMath::Cos(TMath::Pi() / 4) - hits_z * TMath::Sin(TMath::Pi() / 4));
                y2.push_back(hits_y);
                prod_x2.push_back(produced_x);
                prod_y2.push_back(produced_y);
                prod_z2.push_back(produced_z);
            }
            else if ((det_id == 4 && back_size_det_3 == 0) || (det_id == 5 && front_size_det_3 == 0))
            {
                x3.push_back(hits_x * TMath::Cos(-TMath::Pi() / 4) - hits_z * TMath::Sin(-TMath::Pi() / 4));
                y3.push_back(hits_y);
                prod_x3.push_back(produced_x);
                prod_y3.push_back(produced_y);
                prod_z3.push_back(produced_z);
            }
            else if ((det_id == 6 && back_size_det_4 == 0) || (det_id == 7 && front_size_det_4 == 0))
            {
                x4.push_back(hits_x * TMath::Cos(-TMath::Pi() / 4) - hits_z * TMath::Sin(-TMath::Pi() / 4));
                y4.push_back(hits_y);
                prod_x4.push_back(produced_x);
                prod_y4.push_back(produced_y);
                prod_z4.push_back(produced_z);
            }
        }

        // check if "one-hitter"
        if ((front_size_det_1 > 0 && back_size_det_1 == 0) || (back_size_det_1 > 0 && front_size_det_1 == 0))
        {
            for (int i = 0; i < x1.size(); i++)
            {
                if (producedInPhantom(prod_x1[i], prod_y1[i], prod_z1[i]))
                {
                    x1_phantom.push_back(x1[i]);
                    y1_phantom.push_back(y1[i]);
                }
                else
                {
                    x1_outside.push_back(x1[i]);
                    y1_outside.push_back(y1[i]);
                }
            }
        }

        if ((front_size_det_2 > 0 && back_size_det_2 == 0) || (back_size_det_2 > 0 && front_size_det_2 == 0))
        {
            for (int i = 0; i < x2.size(); i++)
            {
                if (producedInPhantom(prod_x2[i], prod_y2[i], prod_z2[i]))
                {
                    x2_phantom.push_back(x2[i]);
                    y2_phantom.push_back(y2[i]);
                }
                else
                {
                    x2_outside.push_back(x2[i]);
                    y2_outside.push_back(y2[i]);
                }
            }
            
        }

        if ((front_size_det_3 > 0 && back_size_det_3 == 0) || (back_size_det_3 > 0 && front_size_det_3 == 0))
        {
            for (int i = 0; i < x3.size(); i++)
            {
                if (producedInPhantom(prod_x3[i], prod_y3[i], prod_z3[i]))
                {
                    x3_phantom.push_back(x3[i]);
                    y3_phantom.push_back(y3[i]);
                }
                else
                {
                    x3_outside.push_back(x3[i]);
                    y3_outside.push_back(y3[i]);
                }
            }

            
        }

        if ((front_size_det_4 > 0 && back_size_det_4 == 0) || (back_size_det_4 > 0 && front_size_det_4 == 0))
        {
            for (int i = 0; i < x4.size(); i++)
            {
                if (producedInPhantom(prod_x4[i], prod_y4[i], prod_z4[i]))
                {
                    x4_phantom.push_back(x4[i]);
                    y4_phantom.push_back(y4[i]);
                }
                else
                {
                    x4_outside.push_back(x4[i]);
                    y4_outside.push_back(y4[i]);
                }
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

    auto graph1_phantom = new TGraph(x1_phantom.size(), x1_phantom.data(), y1_phantom.data());
    graph1_phantom->SetTitle("detector 0; x [mm]; y [mm]");
    auto graph2_phantom = new TGraph(x1_phantom.size(), x2_phantom.data(), y2_phantom.data());
    graph2_phantom->SetTitle("detector 1; x [mm]; y [mm]");
    auto graph3_phantom = new TGraph(x1_phantom.size(), x3_phantom.data(), y3_phantom.data());
    graph3_phantom->SetTitle("detector 2; x [mm]; y [mm]");
    auto graph4_phantom = new TGraph(x1_phantom.size(), x4_phantom.data(), y4_phantom.data());
    graph4_phantom->SetTitle("detector 3; x [mm]; y [mm]");

    auto c2 = new TCanvas((std::string("edge_hitsXY_colored_particle==") + particle).c_str(), (std::string("edge hits (XY colored) for ") + particle + std::string(" (e_min= 100keV)")).c_str());
    c2->SetCanvasSize(1500, 1500);
    c2->Divide(2, 2);
    c2->cd(1);
    graph3_phantom->Draw();
    c2->cd(2);
    graph4_phantom->Draw();
    c2->cd(3);
    graph1_phantom->Draw();
    c2->cd(4);
    graph2_phantom->Draw();

    // auto c1 = new TCanvas((std::string("origin_XY_particle==") + particle).c_str(), (std::string("origin_XY_emin=100keV_particle==") + particle).c_str());
    // c1->Divide(2, 2);
    // c1->cd(1);
    // prod_h3_1i->Draw();
    // c1->cd(2);
    // prod_h4_1i->Draw();
    // c1->cd(3);
    // prod_h1_1i->Draw();
    // c1->cd(4);
    // prod_h2_1i->Draw();
}

// return if given coordinates are in phantom
bool producedInPhantom(double x, double y, double z)
{
    double Phantom_X = 0.0;
    double Phantom_Y = 0.0;
    double Phantom_Z = 0.0;
    double Phantom_DEPTH = 200;   // mm
    double Phantom_RADIUS = 100.; // mm

    if ((std::pow((x - Phantom_X), 2) + std::pow((z - Phantom_Z), 2) <= std::pow(Phantom_RADIUS, 2)) && z <= Phantom_DEPTH / 2 && z >= -Phantom_DEPTH / 2)
    {
        // position is within the Phantom
        return true;
    }
    else
    {
        return false;
    }
}