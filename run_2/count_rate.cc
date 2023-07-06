#include <TFile.h>
#include <TGraph.h>
#include <TTree.h>
#include <string.h>
#include <vector>


void count_rate(){

	int det = 0;
	int strip = 123;
	double e_min = 100000; //unit eV
	
	double psPerEvent = 1/100000000; //1s/10^8ions in this one second
	vector<double> vec_time;

	TFile *file = new TFile("data/final_output/output3.root", "read");
    	TTree *hits = (TTree *)file->Get("hits");

	char particle_name[128];
    	int event_number;
    	int det_id;
	int strip_id;
    	double edep;
	double time;

	
    	hits->SetBranchAddress("name", &particle_name);
    	hits->SetBranchAddress("event", &event_number);
    	hits->SetBranchAddress("Det_ID", &det_id);
	hits->SetBranchAddress("Strip_ID", &strip_id);
    	hits->SetBranchAddress("edep", &edep);
	hits->SetBranchAddress("time", &time);


    	// preload stuff to speed things up
    	hits->LoadBaskets();

    	//int i = 0;
    	int size = hits->GetEntries();
	
	for(int i=0; i<size; i++){
		hits->GetEntry(i);

		if(det_id==det && strip_id==strip){
			double t = psPerEvent*event_number + time;
			vec_time.insert(vec_time.end(), t);
		}

	}


	std::sort(vec_time.begin(), vec_time.end());
	auto g = new TGraph(vec_time.size(), &vec_time[0]);
	g->SetTitle("Time between hits in specific strip");
	g->Draw();
	g->SaveAs("time_elapsed.png");

}
