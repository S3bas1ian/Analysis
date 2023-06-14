#include <TFile.h>
#include <TTree.h>

void ausprobieren()
{

    //File which will be read
    TFile *file = new TFile("mess_1/output_t0.root", "read");
    TTree *hits = (TTree *)file->Get("hits");

    //File, where I write the cluster sizes out
    TFile *outputFile = newTFile("cluster_size.root", "recreate");
    TTree *outputTree = new TTree("cluster_size", "cluster_size");

    int event_size;

    outputTree->Branch("event_size", &event_size, "event_size/I");

    char name_hitscoll[128];
    int event_number;

    hits->SetBranchAddress("Hit_Name", &name_hitscoll);
    hits->SetBranchAddress("event", &event_number);

    while (int i < hits->GetEntries())
    {
        hits->GetEntry(i);
        event_size = hits->Scan("","events=" + str(event_number))
        outputTree->Fill();
        i += event_size;
    }

    outputFile->Write();
    outputFile->Close();
}