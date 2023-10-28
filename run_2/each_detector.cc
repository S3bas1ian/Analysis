/*
creates 2d histograms for cluster size for each detector and 
1d histograms for each detector (front + backside). This is used 
for cluster size
*/

#include <TFile.h>
#include <TTree.h>
#include <string.h>
#include <chrono>


void each_detector(std::string path, std::string particle, Double_t energy_min)
{
    //time measurment of runtime script
    auto start = std::chrono::system_clock::now();

    // File which will be read
    TFile *file = new TFile(path.c_str(), "read");
    TTree *hits = (TTree *)file->Get("hits");

    // 1d histogram for each detector where the total hits (front + backside) are plottet
    auto h1_1d = new TH1I("total_0", "total detector 0; N_{Counts}; #", 10, 0, 10);
    auto h2_1d = new TH1I("total_1", "total detector 1; N_{Counts}; #", 10, 0, 10);
    auto h3_1d = new TH1I("total_2", "total detector 2; N_{Counts}; #", 10, 0, 10);
    auto h4_1d = new TH1I("total_3", "total detector 3; N_{Counts}; #", 10, 0, 10);

    // 1d histogram for gamma, proton, e- and other
    auto h_particles_1d = new TH1I("particles", " ; particle ;#", 4, 0, 4);
    h_particles_1d->Fill("proton", 1);
    h_particles_1d->Fill("deuteron", 1);
    h_particles_1d->Fill("triton", 1);
    h_particles_1d->Fill("e-", 1);
    h_particles_1d->Fill("e+", 1);
    h_particles_1d->Fill("other", 1);



    // 2d histogram for each detector where I seperate between front and backside
    auto h1_2d = new TH2I("0", "detector 0; front side; back side;", 10, 0, 10, 10, 0, 10);
    auto h2_2d = new TH2I("1", "detector 1; front side; back side;", 10, 0, 10, 10, 0, 10);
    auto h3_2d = new TH2I("2", "detector 2; front side; back side;", 10, 0, 10, 10, 0, 10);
    auto h4_2d = new TH2I("3", "detector 3; front side; back side;", 10, 0, 10, 10, 0, 10);

    //char name_hitscoll[128];
    char particle_name[128];
    int event_number;
    int det_id;
    double edep;

    //hits->SetBranchAddress("Hit_Name", &name_hitscoll);
    hits->SetBranchAddress("name", &particle_name);
    hits->SetBranchAddress("event", &event_number);
    hits->SetBranchAddress("Det_ID", &det_id);
    hits->SetBranchAddress("edep", &edep);

    // preload stuff to speed things up
    //hits->LoadBaskets();

    int i = 0;
    int size = hits->GetEntries();  //get amount of entries

    while (i < size) // loop through all events
    {

        //initialize new event
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

        //start with a new event
        hits->GetEntry(i);
        int currEvent = event_number;
        bool sameEvent = true;

        while (sameEvent && i < size) // loop through one full event, increase i while doing so
        {
            hits->GetEntry(i);

            if (event_number == currEvent)
            {
                // we are still looking at the right event

                // only count event if energy is deposited and the particle name matches
                if (edep > energy_min && (particle.compare(std::string(particle_name))==0 || particle.compare("all") == 0)) 
                {
                    // count the event
                    event_size += 1;

                    //fill involved particles
                    if (std::string(particle_name).compare("proton") == 0 || std::string(particle_name).compare("deuteron") == 0
                        || std::string(particle_name).compare("triton") == 0
                        || std::string(particle_name).compare("e-") == 0 || std::string(particle_name).compare("e+") == 0 )
                    {
                        h_particles_1d->Fill(particle_name, 1);
                    }else
                {
                    h_particles_1d->Fill("other", 1);
                }
                
                
                
                //increase counter hit detector
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
            //we are no longer in the "correct" event in will go to the next event
            sameEvent = false;
        }
    }

    if (event_size > 0)
    {

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

// plotting images with root in divided window. Linewidth(3) or even bigger for vivibility
gStyle->SetLabelSize(2);
gStyle->SetTitleSize(2);
gStyle->SetTitleFont(43, "xyz");
gStyle->SetLabelFont(43, "xyz");
gStyle->SetTickLength(0.03, "x");
gStyle->SetTickLength(0.03, "y");
gStyle->SetTickLength(0.03, "z");

auto c1 = new TCanvas((std::string("detectors_particle_") + particle).c_str(), "detectors (e_min= 100keV)");
c1->SetCanvasSize(2880, 1800);
c1->Divide(2, 2);
TPad *p1 = (TPad *)(c1->cd(1));
p1->SetLogz();
h3_2d->Draw("colz");
TPad *p2 = (TPad *)(c1->cd(2));
p2->SetLogz();
h4_2d->Draw("colz");
TPad *p3 = (TPad *)(c1->cd(3));
p3->SetLogz();
h1_2d->Draw("colz");
TPad *p4 = (TPad *)(c1->cd(4));
p4->SetLogz();
h2_2d->Draw("colz");
c1->SetLogz();


auto c2 = new TCanvas((std::string("total_detectors_particle_") + particle).c_str(), "total detectors (e_min= 100keV)");
c2->Divide(2, 2);
c2->cd(1);
h3_1d->SetLineWidth(3);
h3_1d->Draw();
c2->cd(2);
h4_1d->SetLineWidth(3);
h4_1d->Draw();
c2->cd(3);
h1_1d->SetLineWidth(3);
h1_1d->Draw();
c2->cd(4);
h2_1d->SetLineWidth(3);
h2_1d->Draw();

auto stop = std::chrono::system_clock::now();
    cout << "took: "
         << std::chrono::duration_cast<std::chrono::seconds>(stop - start).count()
         << " s \n";


auto c3 = new TCanvas("particles", "particles (e_min= 100keV)");
h_particles_1d->SetLineWidth(3);
h_particles_1d->Draw();

}