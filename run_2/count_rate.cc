#include <TFile.h>
#include <TGraph.h>
#include <TTree.h>
#include <string.h>
#include <vector>

void count_rate()
{

	int det = 0;
	int strip = 123;
	double e_min = 100000; // unit eV

	double psPerEvent = 1e4; //
	vector<double> vec_time_point;

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

	// int i = 0;
	int size = hits->GetEntries();

	for (int i = 0; i < size; i++)
	{
		hits->GetEntry(i);

		if (det_id == det && strip_id == strip)
		{
			double t = psPerEvent * event_number + time;
			vec_time_point.insert(vec_time_point.end(), t);
		}
	}

	std::sort(vec_time_point.begin(), vec_time_point.end());

	int s = vec_time_point.size();
	cout << "average [ps]: " << std::reduce(vec_time_point.begin(), vec_time_point.end()) / static_cast<float>(vec_time_point.size()) << endl;

	double delta_time[s - 1];
	auto h = new TH1D("hist", "delta time", 75, 0, 75);

	for (int i = 0; i < vec_time_point.size() - 1; i++)
	{
		double t = vec_time_point[i + 1] - vec_time_point[i];
		delta_time[i] = t;
		cout << t << endl;
		h->Fill(t);
	}

	auto c1 = new TCanvas("c", "c", 800, 800);
	// g->SetTitle("Time between hits in specific strip");
	h->Draw();
	c1->SaveAs("time_elapsed.png");
}