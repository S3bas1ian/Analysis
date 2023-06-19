#include <TFile.h>
#include <TTree.h>

void ausprobieren()
{

    // stuff for plotting
    auto h1 = new TH1F("front side", "front side; N_{Counts}; #", 10, 0, 10);
    auto h2 = new TH1F("back side", "back side; N_{Counts}; #", 10, 0, 10);
    auto h3 = new TH1F("total", "total per detector; N_{Counts}; #", 20, 0, 20);

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
    int size = hits->GetEntries();

    cout << size << endl;

    while (i < size)
    {
        front_size = 0;
        back_size = 0;
        hits->GetEntry(i);
        int currEvent = event_number;

        event_size = 0;

        bool sameEvent = true;

        while (sameEvent && i < size)
        {
            hits->GetEntry(i);
            if (event_number == currEvent)
            {
                event_size += 1;
                if (det_id % 2 == 0)
                {
                    front_size += 1;
                }
                else
                {
                    back_size += 1;
                }

                i += 1;
            }
            else
            {
                sameEvent = false;
            }
        }

        h1->Fill(front_size);
        h2->Fill(back_size);
        h3->Fill(event_size);
        outputTree->Fill();
    }

    outputFile->Write();
    outputFile->Close();

    auto c1 = new TCanvas("c1", "hits per side");
    c1->Divide(2, 1);
    c1->cd(1);

    h1->Draw();

    c1->cd(2);
    h2->Draw();

    auto c2 = new TCanvas("c2", "total hits");

    h3->Draw();

    // auto legend = new TLegend(0.7, 0.5, 1.0, 0.3);
    // legend->SetHeader("# hits per event", "C");
    // legend->AddEntry(h3, "All hits", "l");
    // legend->AddEntry(h1, "hits from front sides", "l");
    // legend->AddEntry(h2, "hits from back sides", "l");

    // legend->Draw();
}
