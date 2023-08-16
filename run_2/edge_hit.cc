#include <TFile.h>
#include <TTree.h>
#include <string.h>

void edge_hit(std::string path, std::string particle)
{

    // constants
    double energy_min = 100000; // eV

    TFile *file = new TFile(path.c_str(), "read");
    TTree *hits = (TTree *)file->Get("hits");

    auto h1_1d = new TH1I("edges_det_0", "edges detector 0; N_{Counts}; #", 2, 0, 0);
    // auto h2_1d = new TH1I("edges_det_1", "edges detector 1; N_{Counts}; #", 2, 0, 1);
    // auto h3_1d = new TH1I("edges_det_2", "edges detector 2; N_{Counts}; #", 2, 0, 1);
    // auto h4_1d = new TH1I("edges_det_3", "edges detector 3; N_{Counts}; #", 2, 0, 1);

    char particle_name[128];
    Int_t event_number;
    Int_t det_id;
    Int_t strip_id;
    Double_t edep;

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
        Int_t last_strip_id_1;
        Int_t last_strip_id_2;
        Int_t last_strip_id_3;
        Int_t last_strip_id_4;

        hits->GetEntry(i);
        int currEvent = event_number;
        bool sameEvent = true;
        std::vector<int> strips;

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
                strips.push_back(strip_id);
            }else if (strips.size()>0){
                strips.resize(0);
            }

        }

        // check if "one-hitter"
        if ((det_id==0 && back_size_det_1 == 0) || (det_id==1 && front_size_det_1 == 0))
        {
            for(int s:strips){
                h1_1d->Fill(s);
            }
        }
    }

    auto c2 = new TCanvas((std::string("edge_hits_particle==") + particle).c_str(), (std::string("edge hits for ") + particle + std::string(" (e_min= 100keV")).c_str());
    // c2->Divide(2, 2);
    // c2->cd(1);
    // h3_1d->Draw();
    // c2->cd(2);
    // h4_1d->Draw();
    // c2->cd(3);
    h1_1d->Draw();
    // c2->cd(4);
    // h2_1d->Draw();
}