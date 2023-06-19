#include <TFile.h>
#include <TTree.h>

void ausprobieren()
{

    // File which will be read
    TFile *file = new TFile("mess_1/output_t0.root", "read");
    TTree *hits = (TTree *)file->Get("hits");

    // File, where I write the cluster sizes out
    TFile *outputFile = new TFile("cluster_size.root", "recreate");
    TTree *outputTree = new TTree("cluster_size", "cluster_size");

    int event_size;
    int front_size;
    int back_size;

    outputTree->Branch("event_size", &event_size, "event_size/I");
    outputTree->Branch("front_size", &front_size, "front_size/I");
    outputTree->Branch("back_size", &back_size, "back_size/I");

    char name_hitscoll[128];
    int event_number;
    int det_id;

    hits->SetBranchAddress("Hit_Name", &name_hitscoll);
    hits->SetBranchAddress("event", &event_number);
    hits->SetBranchAddress("Det_ID", &det_id);

    hits->LoadBaskets();

    int i = 0;
    while (i < 50000)    //hits->GetEntries()
    {
        front_size = 0;
        back_size = 0;
        hits->GetEntry(i);
        int currEvent = event_number;

        event_size = 0;

        bool sameEvent = true;

        while (sameEvent)
        {
            hits->GetEntry(i);
            if (event_number == currEvent)
            {
                event_size +=1;
                if (det_id % 2 == 0)
                {
                    front_size += 1;
                }
                else
                {
                    back_size += 1;
                }

                i+=1;
            }
            else
            {
                sameEvent = false;
            }
        }

        outputTree->Fill();
    }

    outputFile->Write();
    outputFile->Close();
}