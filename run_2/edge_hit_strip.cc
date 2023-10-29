#include <TFile.h>
#include <TTree.h>
#include <string.h>

void edge_hit_strip(std::string path, std::string particle, Double_t energy_min)
{

    //Trees
    TFile *file = new TFile(path.c_str(), "read");
    TTree *hits = (TTree *)file->Get("hits");

    //histogramms
    auto h1_1d = new TH1I("edges_det_0", "detector 0; strip; #", 1024, 0, 1024);
    auto h2_1d = new TH1I("edges_det_1", "detector 1; strip; #", 1024, 0, 1024);
    auto h3_1d = new TH1I("edges_det_2", "detector 2; strip; #", 1024, 0, 1024);
    auto h4_1d = new TH1I("edges_det_3", "detector 3; strip; #", 1024, 0, 1024);

    //variables for tree
    char particle_name[128];
    Int_t event_number;
    Int_t det_id;
    Int_t strip_id;
    Double_t edep;

    //connecting variables and tree
    hits->SetBranchAddress("name", &particle_name);
    hits->SetBranchAddress("event", &event_number);
    hits->SetBranchAddress("Det_ID", &det_id);
    hits->SetBranchAddress("Strip_ID", &strip_id);
    hits->SetBranchAddress("edep", &edep);

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

        //initialize for each new event
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
        event_size = 0;

        hits->GetEntry(i);
        int currEvent = event_number;
        bool sameEvent = true;
        std::vector<int> strips1, strips2, strips3, strips4;

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

            //only add if we are in the case of "one side hitters" still
            if((det_id==0 && back_size_det_1 == 0) || (det_id==1 && front_size_det_1 == 0)){
                strips1.push_back(strip_id);
            } else if((det_id==2 && back_size_det_2 == 0) || (det_id==3 && front_size_det_2 == 0)){
                strips2.push_back(strip_id);
            } else if((det_id==4 && back_size_det_3 == 0) || (det_id==5 && front_size_det_3 == 0)){
                strips3.push_back(strip_id);
            }else if((det_id==6 && back_size_det_4 == 0) || (det_id==7 && front_size_det_4 == 0)){
                strips4.push_back(strip_id);
            }

        }

        // check if "one-hitter"
        if ((front_size_det_1 > 0 && back_size_det_1 == 0) || (back_size_det_1 > 0 && front_size_det_1 == 0))
        {
            for(int s:strips1){
                h1_1d->Fill(s);
            }
        }

        if ((front_size_det_2 > 0 && back_size_det_2 == 0) || (back_size_det_2 > 0 && front_size_det_2 == 0))
        {
            for(int s:strips2){
                h2_1d->Fill(s);
            }
        }

        if ((front_size_det_3 > 0 && back_size_det_3 == 0) || (back_size_det_3 > 0 && front_size_det_3 == 0))
        {
            for(int s:strips3){
                h3_1d->Fill(s);
            }
        }

        if ((front_size_det_4 > 0 && back_size_det_4 == 0) || (back_size_det_4 > 0 && front_size_det_4 == 0))
        {
            for(int s:strips4){
                h4_1d->Fill(s);
            }
        }
        //delete strips for next event block
        strips1.resize(0);
        strips2.resize(0);
        strips3.resize(0);
        strips4.resize(0);
    }


    gStyle->SetLabelSize(50, "xyz");
    gStyle->SetTitleSize(70, "xyz");
    gStyle->SetTitleSize(0.09f, "t");
    gStyle->SetCanvasDefW(2880);
    gStyle->SetCanvasDefH(1800);
    gStyle->SetTitleFont(43, "xyz");
    gStyle->SetLabelFont(43, "xyz");
    gStyle->SetTickLength(0.04, "xyz");
    gStyle->SetLineWidth(4);
    gStyle->SetOptStat(0);
    gStyle->SetPadBottomMargin(0.17);
    gStyle->SetPadLeftMargin(0.11);
    gStyle->SetPadRightMargin(0.13);

    //plotting
    auto c2 = new TCanvas((std::string("edge_hitsStrip_particle_") + particle).c_str(), (std::string("edge hits (strip) for ") + particle + std::string(" (e_min= 100keV)")).c_str());
    c2->SetCanvasSize(2880, 1800);
    c2->Divide(2, 2);
    c2->cd(1);
    h3_1d->SetTitleSize(70, "xyz");
    h3_1d->SetLabelSize(59, "xyz");
    h3_1d->SetLabelFont(43, "xyz");
    h3_1d->SetTitleFont(43, "xyz");
    h3_1d->GetYaxis()->SetNdivisions(6, 3, false);
    h3_1d->Draw();
    c2->cd(2);
    h4_1d->SetTitleSize(70, "xyz");
    h4_1d->SetLabelSize(59, "xyz");
    h4_1d->SetLabelFont(43, "xyz");
    h4_1d->SetTitleFont(43, "xyz");
    h4_1d->GetYaxis()->SetNdivisions(6, 3, false);
    h4_1d->Draw();
    c2->cd(3);
    h1_1d->SetTitleSize(70, "xyz");
    h1_1d->SetLabelSize(59, "xyz");
    h1_1d->SetLabelFont(43, "xyz");
    h1_1d->SetTitleFont(43, "xyz");
    h1_1d->GetYaxis()->SetNdivisions(6, 3, false);
    h1_1d->Draw();
    c2->cd(4);
    h2_1d->SetTitleSize(70, "xyz");
    h2_1d->SetLabelSize(59, "xyz");
    h2_1d->SetLabelFont(43, "xyz");
    h2_1d->SetTitleFont(43, "xyz");
    h2_1d->GetYaxis()->SetNdivisions(6, 3, false);
    h2_1d->Draw();
}