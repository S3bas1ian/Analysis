#include <TFile.h>
#include <TTree.h>
#include <string.h>

void each_detector()
{

    // File which will be read
    TFile *file = new TFile("mess_1/full_output.root", "read");
    TTree *hits = (TTree *)file->Get("hits");

    // 1d histogram for each detector where the total hits (front + backside) are plottet
    auto h1_1d = new TH1I("total_0", "total detector 0; N_{Counts}; #", 10, 0, 10);
    auto h2_1d = new TH1I("total_1", "total detector 1; N_{Counts}; #", 10, 0, 10);
    auto h3_1d = new TH1I("total_2", "total detector 2; N_{Counts}; #", 10, 0, 10);
    auto h4_1d = new TH1I("total_3", "total detector 3; N_{Counts}; #", 10, 0, 10);

    // 1d histogram for gamma, proton, e- and other
    auto h_particles_1d = new TH1I("particles", "total events per particle; particle ;#", 4, 0, 4);

    // 2d histogram for each detector where I seperate between front and backside
    auto h1_2d = new TH2I("0", "detector 0; front side; back side;", 10, 0, 10, 10, 0, 10);
    auto h2_2d = new TH2I("1", "detector 1; front side; back side;", 10, 0, 10, 10, 0, 10);
    auto h3_2d = new TH2I("2", "detector 2; front side; back side;", 10, 0, 10, 10, 0, 10);
    auto h4_2d = new TH2I("3", "detector 3; front side; back side;", 10, 0, 10, 10, 0, 10);

    char name_hitscoll[128];
    char particle_name[128];
    int event_number;
    int det_id;
    double edep;

    hits->SetBranchAddress("Hit_Name", &name_hitscoll);
    hits->SetBranchAddress("name", &particle_name);
    hits->SetBranchAddress("event", &event_number);
    hits->SetBranchAddress("Det_ID", &det_id);
    hits->SetBranchAddress("edep", &edep);

    // preload stuff to speed things up
    hits->LoadBaskets();

    int i = 0;
    int size = hits->GetEntries();

    while (i < size) // loop through all events
    {

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

        hits->GetEntry(i);
        int currEvent = event_number;
        bool sameEvent = true;

        while (sameEvent && i < size) // loop through one full event, increase i while doing so
        {
            hits->GetEntry(i);

            if (event_number == currEvent)
            {
                // we are still looking at the right event

                if (edep > 0) // only count event if energy is deposited
                {
                    // count the event
                    event_size += 1;

                    if (det_id == 0)
                    {
                        event_size_det_1 += 1;
                        front_size_det_1 += 1;
                    }
                    else if (det_id == 1)
                    {
                        event_size_det_1 += 1;
                        back_size_det_1 += 1;
                    }
                    else if (det_id == 2)
                    {
                        event_size_det_2 += 1;
                        front_size_det_2 += 1;
                    }
                    else if (det_id == 3)
                    {
                        event_size_det_2 += 1;
                        back_size_det_2 += 1;
                    }
                    else if (det_id == 4)
                    {
                        event_size_det_3 += 1;
                        front_size_det_3 += 1;
                    }
                    else if (det_id == 5)
                    {
                        event_size_det_3 += 1;
                        back_size_det_3 += 1;
                    }
                    else if (det_id == 6)
                    {
                        event_size_det_4 += 1;
                        front_size_det_4 += 1;
                    }
                    else if (det_id == 7)
                    {
                        event_size_det_4 += 1;
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
        }

        if (event_size > 0)
        {

            // count amount of events with gamma, proton etc.
            if (strcmp(particle_name, "gamma") == 0)
            {
                // particle_size[0] += 1;
                h_particles_1d->Fill("gamma", 1);
            }
            else if (strcmp(particle_name, "proton") == 0)
            {
                // particle_size[1] += 1;
                h_particles_1d->Fill("proton", 1);
            }
            else if (strcmp(particle_name, "e-") == 0)
            {
                // particle_size[2] += 1;
                h_particles_1d->Fill("e-", 1);
            }
            else
            {
                // particle_size[3] += 1;
                h_particles_1d->Fill("other", 1);
            }

            // Fill histogramms only if there is an event to avoid 0 pile
            if (event_size_det_1 > 0)
            {
                h1_1d->Fill(event_size_det_1);
                h1_2d->Fill(front_size_det_1, back_size_det_1);
            }
            else if (event_size_det_2 > 0)
            {
                h2_1d->Fill(event_size_det_2);
                h2_2d->Fill(front_size_det_2, back_size_det_2);
            }
            else if (event_size_det_3 > 0)
            {
                h3_1d->Fill(event_size_det_3);
                h3_2d->Fill(front_size_det_3, back_size_det_3);
            }
            else if (event_size_det_4 > 0)
            {
                h4_1d->Fill(event_size_det_4);
                h4_2d->Fill(front_size_det_4, back_size_det_4);
            }
        }
    }

    // plotting and saving images with root
    auto c1 = new TCanvas("c1", "detectors");
    c1->Divide(2, 2);
    c1->cd(1);
    h3_2d->Draw("colz");
    c1->cd(2);
    h4_2d->Draw("colz");
    c1->cd(3);
    h1_2d->Draw("colz");
    c1->cd(4);
    h2_2d->Draw("colz");

    c1->SaveAs("2d_hist_detectors.png");

    auto c2 = new TCanvas("c2", "total detectors");
    c2->Divide(2, 2);
    c2->cd(1);
    h3_1d->Draw();
    c2->cd(2);
    h4_1d->Draw();
    c2->cd(3);
    h1_1d->Draw();
    c2->cd(4);
    h2_1d->Draw();

    // gStyle->SetImageScaling(3); should create a high res image, but doesnt work
    c2->SaveAs("total_detectors.png");

    auto c3 = new TCanvas("c3", "particles");
    h_particles_1d->Draw();
    c3->SaveAs("particle_overview.png");
}