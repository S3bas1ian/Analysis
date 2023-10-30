#include <TFile.h>
#include <TTree.h>
#include <string.h>
#include <TMath.h>
#include <TGraph.h>

bool producedInPhantom(double x, double y, double z);

void edge_hit_xy_tgraph(std::string path, std::string particle, Double_t energy_min)
{

    TFile *file = new TFile(path.c_str(), "read");
    TTree *hits = (TTree *)file->Get("hits");

    //Hits on detector for particles from phantom/not from phantom
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

    //origigin of particles that are not from the phantom
    std::vector<double> outsider_x, outsider_y, outsider_z;

    //variables that will get connected
    char particle_name[128];
    Int_t event_number;
    Int_t det_id;
    Int_t strip_id;
    Double_t edep, energy;
    Double_t hits_x, hits_y, hits_z;
    Double_t produced_x, produced_y, produced_z;

    //connecting the variables
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

    //events per full detector (both sides) for each event block
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

        //initialize for each event block
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

        //initialize needed verctors for each event block
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

            //if still one side hitter, than add to the belonging detector
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
                    outsider_x.push_back(prod_x1[i]);
                    outsider_y.push_back(prod_y1[i]);
                    outsider_z.push_back(prod_z1[i]);
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
                    outsider_x.push_back(prod_x2[i]);
                    outsider_y.push_back(prod_y2[i]);
                    outsider_z.push_back(prod_z2[i]);
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
                    outsider_x.push_back(prod_x3[i]);
                    outsider_y.push_back(prod_y3[i]);
                    outsider_z.push_back(prod_z3[i]);
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
                    outsider_x.push_back(prod_x4[i]);
                    outsider_y.push_back(prod_y4[i]);
                    outsider_z.push_back(prod_z4[i]);
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

    //plotting hits on detector

    gStyle->SetLabelSize(60, "xyz");
    gStyle->SetTitleSize(70, "xyz");
    gStyle->SetTitleFont(43, "xyz");
    gStyle->SetTitleFontSize(0.08f);
    gStyle->SetLabelFont(43, "xyz");
    gStyle->SetTickLength(0.04, "xyz");
    gStyle->SetLineWidth(5);
    gStyle->SetFrameLineWidth(5);
    gStyle->SetOptStat(0);
    gStyle->SetPadBottomMargin(0.15);
    gStyle->SetPadLeftMargin(0.17);
    gStyle->SetPadRightMargin(0.02);
    gStyle->SetPadTopMargin(0.08);
    gStyle->SetTitleOffset(0.015, "y");
    gStyle->SetTitleOffset(0.1, "t");


    auto graph1_phantom = new TGraph(x1_phantom.size(), x1_phantom.data(), y1_phantom.data());
    auto graph1_outside = new TGraph(x1_outside.size(), x1_outside.data(), y1_outside.data());

    auto graph2_phantom = new TGraph(x2_phantom.size(), x2_phantom.data(), y2_phantom.data());
    auto graph2_outside = new TGraph(x2_outside.size(), x2_outside.data(), y2_outside.data());

    auto graph3_phantom = new TGraph(x3_phantom.size(), x3_phantom.data(), y3_phantom.data());
    auto graph3_outside = new TGraph(x3_outside.size(), x3_outside.data(), y3_outside.data());

    auto graph4_phantom = new TGraph(x4_phantom.size(), x4_phantom.data(), y4_phantom.data());
    auto graph4_outside = new TGraph(x4_outside.size(), x4_outside.data(), y4_outside.data());

    auto c1 = new TCanvas((std::string("edge_hitsXY_colored_particle==") + particle).c_str(), (std::string("edge hits (XY colored) for ") + particle + std::string(" (e_min= 100keV)")).c_str());


    //particles with origin inside or outside the phantom have different colors
    TMultiGraph *mg1 = new TMultiGraph();
    mg1->SetTitle("detector 0; x [mm]   ; y [mm]   ");
    graph1_phantom->SetMarkerColor(kBlue);
    graph1_outside->SetMarkerColor(kRed);

    mg1->Add(graph1_phantom);
    mg1->Add(graph1_outside);

    TMultiGraph *mg2 = new TMultiGraph();
    mg2->SetTitle("detector 1; x [mm]   ; y [mm]   ");
    graph2_phantom->SetMarkerColor(kBlue);
    graph2_outside->SetMarkerColor(kRed);

    mg2->Add(graph2_phantom);
    mg2->Add(graph2_outside);

    TMultiGraph *mg3 = new TMultiGraph();
    mg3->SetTitle("detector 2; x [mm]   ; y [mm]   ");
    graph3_phantom->SetMarkerColor(kBlue);
    graph3_outside->SetMarkerColor(kRed);

    mg3->Add(graph3_phantom);
    mg3->Add(graph3_outside);

    TMultiGraph *mg4 = new TMultiGraph();
    mg4->SetTitle("detector 3; x [mm]   ; y [mm]   ");
    graph4_phantom->SetMarkerColor(kBlue);
    graph4_outside->SetMarkerColor(kRed);

    mg4->Add(graph4_phantom);
    mg4->Add(graph4_outside);

    c1->SetCanvasSize(1500, 1500);
    c1->Divide(2, 2);
    c1->cd(1);
    mg3->GetXaxis()->SetNdivisions(7,2,false);
    mg3->GetXaxis()->SetTitleFont(43);
    mg3->GetXaxis()->SetLabelFont(43);
    mg3->GetXaxis()->SetTitleSize(53);
    mg3->GetXaxis()->SetLabelSize(50);
    mg3->GetYaxis()->SetNdivisions(7,2,false);
    mg3->GetYaxis()->SetTitleFont(43);
    mg3->GetYaxis()->SetLabelFont(43);
    mg3->GetYaxis()->SetTitleSize(53);
    mg3->GetYaxis()->SetLabelSize(50);
    mg3->GetYaxis()->SetTitleOffset(1.1);
    mg3->Draw("AP");
    c1->cd(2);
    mg4->GetXaxis()->SetNdivisions(7,2,false);
    mg4->GetXaxis()->SetTitleFont(43);
    mg4->GetXaxis()->SetLabelFont(43);
    mg4->GetXaxis()->SetTitleSize(53);
    mg4->GetXaxis()->SetLabelSize(50);
    mg4->GetYaxis()->SetNdivisions(7,2,false);
    mg4->GetYaxis()->SetTitleFont(43);
    mg4->GetYaxis()->SetLabelFont(43);
    mg4->GetYaxis()->SetTitleSize(53);
    mg4->GetYaxis()->SetLabelSize(50);
    mg4->GetYaxis()->SetTitleOffset(1.1);
    mg4->Draw("AP");
    c1->cd(3);
    mg1->GetXaxis()->SetNdivisions(7,2,false);
    mg1->GetXaxis()->SetTitleFont(43);
    mg1->GetXaxis()->SetLabelFont(43);
    mg1->GetXaxis()->SetTitleSize(53);
    mg1->GetXaxis()->SetLabelSize(50);
    mg1->GetYaxis()->SetNdivisions(7,2,false);
    mg1->GetYaxis()->SetTitleFont(43);
    mg1->GetYaxis()->SetLabelFont(43);
    mg1->GetYaxis()->SetTitleSize(53);
    mg1->GetYaxis()->SetLabelSize(50);
    mg1->GetYaxis()->SetTitleOffset(1.1);
    mg1->Draw("AP");
    c1->cd(4);
    mg2->GetXaxis()->SetNdivisions(7,2,false);
    mg2->GetXaxis()->SetTitleFont(43);
    mg2->GetXaxis()->SetLabelFont(43);
    mg2->GetXaxis()->SetTitleSize(53);
    mg2->GetXaxis()->SetLabelSize(50);
    mg2->GetYaxis()->SetNdivisions(7,2,false);
    mg2->GetYaxis()->SetTitleFont(43);
    mg2->GetYaxis()->SetLabelFont(43);
    mg2->GetYaxis()->SetTitleSize(53);
    mg2->GetYaxis()->SetLabelSize(50);
    mg2->GetYaxis()->SetTitleOffset(1.1);
    mg2->Draw("AP");


    //plotting view 100
    auto c2 = new TCanvas((std::string("position_outsider_100_particle==") + particle).c_str(), 
                            (std::string("position_outsider_100_particle==") + particle + std::string(" (e_min= 100keV)")).c_str());
    auto outsider_100 = new TGraph(outsider_y.size(), outsider_y.data(), outsider_z.data());
    outsider_100->SetTitle("yz-plane; y [mm]; z [mm]");
    outsider_100->Draw("AP");


    //plotting view 010
    auto c3 = new TCanvas((std::string("position_outsider_010_particle==") + particle).c_str(), 
                            (std::string("position_outsider_010_particle==") + particle + std::string(" (e_min= 100keV)")).c_str());
    auto outsider_010 = new TGraph(outsider_x.size(), outsider_x.data(), outsider_z.data());
    outsider_010->SetTitle("xz-plane; x [mm]; z [mm]");
    outsider_010->Draw("AP");

    //plotting view 001
    auto c4 = new TCanvas((std::string("position_outsider_001_particle==") + particle).c_str(), 
                            (std::string("position_outsider_001_particle==") + particle + std::string(" (e_min= 100keV)")).c_str());
    auto outsider_001 = new TGraph(outsider_x.size(), outsider_x.data(), outsider_y.data());
    outsider_001->SetTitle("xy-plane; x [mm]; y [mm]");
    outsider_001->Draw("AP");


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