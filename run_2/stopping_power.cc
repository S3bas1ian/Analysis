#include <TFile.h>
#include <TTree.h>
#include <string.h>

// calculates the stopping power (dE vs E) for a specific particle

void stopping_power(std::string path, std::string particle)
{
    // constants
    double energy_min = 100000; // eV

    //root file and trees
    TFile *file = new TFile(path.c_str(), "read");
    TTree *hits = (TTree *)file->Get("hits");
    TH2D *hist = new TH2D("stopping power", "stopping power",
                          1000, 0, 300e3, 1000, 0, 1500);

    //needed variables from the tree
    char particle_name[128];
    Int_t event_number;
    Int_t det_id;
    Double_t edep, energy;

    //connect those variables to the tree
    hits->SetBranchAddress("name", &particle_name);
    hits->SetBranchAddress("event", &event_number);
    hits->SetBranchAddress("Det_ID", &det_id);
    hits->SetBranchAddress("edep", &edep);

    //needed for the loop through the tree
    int i = 0;
    int size = hits->GetEntries();

    int front_size_det_1;
    int front_size_det_2;
    int front_size_det_3;
    int front_size_det_4;

    int back_size_det_1;
    int back_size_det_2;
    int back_size_det_3;
    int back_size_det_4;

    //stopping power in each detector
    std::vector<Double_t> dE1;
    std::vector<Double_t> E1;
    std::vector<Double_t> dE2;
    std::vector<Double_t> E2;
    std::vector<Double_t> dE3;
    std::vector<Double_t> E3;
    std::vector<Double_t> dE4;
    std::vector<Double_t> E4;

    while (i < size)
    {
        front_size_det_1 = 0;
        front_size_det_2 = 0;
        front_size_det_3 = 0;
        front_size_det_4 = 0;

        back_size_det_1 = 0;
        back_size_det_2 = 0;
        back_size_det_3 = 0;
        back_size_det_4 = 0;

        //after each event block the stopping power is added to the 
        //histogram. Now the stopping power has to be emptied, so the
        //next event can be calculated
        dE1.resize(0);
        E1.resize(0);
        dE2.resize(0);
        E2.resize(0);
        dE3.resize(0);
        E3.resize(0);
        dE4.resize(0);
        E4.resize(0);

        hits->GetEntry(i);
        int currEvent = event_number;
        bool sameEvent = true;

        while (sameEvent && i < size)
        {
            hits->GetEntry(i);

            if (event_number == currEvent)
            {

                if (std::string(particle_name).compare(particle) == 0 )
                {
                    if (det_id == 0)
                    {
                        front_size_det_1 += 1;
                        dE1.push_back(edep);
                        E1.push_back(energy);
                    }
                    else if (det_id == 1)
                    {
                        back_size_det_1 += 1;
                        dE1.push_back(edep);
                        E1.push_back(energy);
                    }
                    else if (det_id == 2)
                    {
                        front_size_det_2 += 1;
                        dE2.push_back(edep);
                        E2.push_back(energy);
                    }
                    else if (det_id == 3)
                    {
                        back_size_det_2 += 1;
                        dE2.push_back(edep);
                        E2.push_back(energy);
                    }
                    else if (det_id == 4)
                    {
                        front_size_det_3 += 1;
                        dE3.push_back(edep);
                        E3.push_back(energy);
                    }
                    else if (det_id == 5)
                    {
                        back_size_det_3 += 1;
                        dE3.push_back(edep);
                        E3.push_back(energy);
                    }
                    else if (det_id == 6)
                    {
                        front_size_det_4 += 1;
                        dE4.push_back(edep);
                        E4.push_back(energy);
                    }
                    else if (det_id == 7)
                    {
                        back_size_det_4 += 1;
                        dE4.push_back(edep);
                        E4.push_back(energy);
                    }
                    i += 1;
                }
                else
                {
                    i++;
                }
            }
            else
            {
                sameEvent = false;
            }
        }

        //add stopping power only if we have no "one side hitters"
        if (front_size_det_1 > 0 && back_size_det_1 > 0)
        {
            for (int j = 0; i < E1.size(); j++)
            {
                hist->Fill(E1[j] / 1e3, dE1[j] / 1e3);
            }
        }
        if (front_size_det_2 > 0 && back_size_det_2 > 0)
        {
            for (int j = 0; i < E2.size(); j++)
            {
                hist->Fill(E2[j] / 1e3, dE2[j] / 1e3);
            }
        }
        if (front_size_det_3 > 0 && back_size_det_3 > 0)
        {
            for (int j = 0; i < E3.size(); j++)
            {
                hist->Fill(E3[j] / 1e3, dE3[j] / 1e3);
            }
        }
        if (front_size_det_4 > 0 && back_size_det_4 > 0)
        {
            for (int j = 0; i < E4.size(); j++)
            {
                hist->Fill(E4[j] / 1e3, dE4[j] / 1e3);
            }
        }
    }

    //create canvas with all information
    auto c1 = new TCanvas((std::string("stopping_power_particle=") +
                           particle)
                              .c_str(),
                          (std::string("stopping_power_particle=") +
                           particle)
                              .c_str());

    hist->SetTitle((std::string("Stopping Power for ") + 
                    particle).c_str());
    hist->SetXTitle("Energy [keV]");
    hist->SetYTitle("# Delta E [keV]");
    hist->Draw();
}