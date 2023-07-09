#include <TFile.h>
#include <TGraph.h>
#include <TTree.h>
#include <string.h>
#include <vector>


vector<double> get_Mean_and_Stdv(int d, int s, TTree *hits) // should return mean and stdv
{

	double e_min = 100000;	 // unit eV
	double psPerEvent = 1e4; // unit pS
	vector<double> vec_time_point;
	int size = hits->GetEntries();

	// variables that are connected to the root file
	char particle_name[128];
	int event_number;
	int det_id;
	int strip_id;
	double edep;
	double time;

	// connect variables with the branch from the root file
	hits->SetBranchAddress("name", &particle_name);
	hits->SetBranchAddress("event", &event_number);
	hits->SetBranchAddress("Det_ID", &det_id);
	hits->SetBranchAddress("Strip_ID", &strip_id);
	hits->SetBranchAddress("edep", &edep);
	hits->SetBranchAddress("time", &time);

	// preload stuff to speed things up
	hits->LoadBaskets();

	// iterate through the whole root file
	for (int i = 0; i < size; i++)
	{
		hits->GetEntry(i);

		// if the current entry is on the specific strip
		if (det_id == d && strip_id == s && edep>0)
		{
			// then add the time point to the vector
			double t = psPerEvent * event_number + time;
			vec_time_point.push_back(t);
		}
	}

	// sort the time point vector
	std::sort(vec_time_point.begin(), vec_time_point.end());

	vector<double> delta_time;
	// auto h = new TH1D("hist", "delta time", 100, 0, 2);

	// calculate the time between two events
	for (int i = 0; i < vec_time_point.size() - 1; i++)
	{
		double t = vec_time_point[i + 1] - vec_time_point[i];
		delta_time.push_back(t);
		// h->Fill(t);
	}

	// calculate mean and stdv and return it
	double sum = std::accumulate(std::begin(delta_time), std::end(delta_time), 0.0);
	double m = sum / delta_time.size();

	double accum = 0.0;
	std::for_each(std::begin(delta_time), std::end(delta_time), [&](const double d)
				  { accum += (d - m) * (d - m); });

	double stdev = sqrt(accum / (delta_time.size() - 1));
	vector<double> out;
	out.push_back(m); 
	out.push_back(stdev);
	return out;
}

void count_rate()
{

	int det[] = {0, 1, 6, 7};
	int strip[] = {0, 1, 2, 200, 555, 700, 1021, 1022, 1023};

	TFile *file = new TFile("data/final_output/output3.root", "read");
	TTree *hits = (TTree *)file->Get("hits");

	TFile *delta_time_file = new TFile("data/delta_time.root", "recreate");
	TTree* output_tree = new TTree("delta_time", "delta_time");

	//vector<double> all_times;
	//vector<double> all_Stdv;

	double t, stdv;
	int dt, strp; 

	output_tree->Branch("dt", &dt, "dt/I");
	output_tree->Branch("strp", &strp, "strp/I");
	output_tree->Branch("t", &t, "t/D");
	output_tree->Branch("stdv", &stdv, "stdv/D");

	// iterate through the wanted detectors and strips
	for (int d : det)
	{
		for (int s : strip)
		{
			vector<double> o;

			// calculate for each det/strip combnination the mean and stdv
			o = get_Mean_and_Stdv(d, s, hits);
			//all_times.push_back(o[0]);

			t = o[0];
			stdv = o[1];
			dt = d;
			strp = s;
			output_tree->Fill();

			cout << "------------ detector: " << d << "and strips: " << s << "------------" << endl;
			cout << "average [ps]: " << o[0] << endl;
			cout << "std deviation [ps]: " << o[1] << endl;


		}
	}

	delta_time_file->Write();
	delta_time_file->Close();

	// auto c1 = new TCanvas("c", "c", 800, 800);
	// // g->SetTitle("Time between hits in specific strip");
	// h->Draw();
	// c1->SaveAs("time_elapsed.png");
}


