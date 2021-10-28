#include "info.h"

using namespace std;

/**
 * Funtion to pull the first <num_colums> data from each pass comma separated
 * string
 */
vector<string> take_columns(int num_columns, string row) {
	int pos;		     // keep track of the column position
	string data;		     // column,row data holder
	vector<string> data_points;  // vector to contain all data points
	int current_column = 1;	     // start at column 1
	// iterate through each comma point and pull out the data.  Stop when
	// there are no more commas or the num_columns is reached
	while ((pos = row.find(',')) != string::npos &&
	       current_column <= num_columns) {
		data = row.substr(0, pos);
		data_points.push_back(data);
		// remove the data from the string up to the comma
		row.erase(0, pos + 1);
		current_column++;
	}
	// If the the last column was the num_columns then push the last string
	// data of the row
	if (current_column == num_columns) {
		data_points.push_back(row);
	} else if (current_column < num_columns) {  // If there are not enough
						    // columns, exit with error
		cout << "Not enough columns" << endl;
		exit(1);
	}
	return data_points;
}

/**
 * Pulls in the given sample barcode file and pushes the data into an
 * unodered_map. This map is later used to convert DNA barcode to sample name.
 * Additionally, an unordered_set of sequences is created to error correct for
 * any sequencing errors.
 */
void BarcodeConversion::sample_barcode_conversion(const string barcode_path) {
	ifstream barcode_file;
	barcode_file.open(barcode_path);
	if (!barcode_file.is_open()) {
		cout << barcode_path << " not found" << endl;
		exit(1);
	}

	string row;
	std::getline(barcode_file, row);  // The first row should be the header
	for (string row; std::getline(barcode_file, row);) {
		vector<string> row_vec = take_columns(2, row);
		samples_barcode_hash.insert(
		    std::pair<string, string>(row_vec[0], row_vec[1]));
		samples_seqs.insert(row_vec[0]);
	}
}

/**
 * Pulls in the given counted barcodes file and pushes the data into an
 * unodered_map for each sequential barcode. These maps are later used to
 * convert DNA barcode to sample name. Additionally, unordered_sets of the
 * sequences are created to error correct for any sequencing errors.
 */
void BarcodeConversion::barcode_file_conversion(const string barcode_path) {
	// Open the file and handle not found errors
	ifstream barcode_file;
	barcode_file.open(barcode_path);
	if (!barcode_file.is_open()) {
		cout << barcode_path << " not found" << endl;
		exit(1);
	}

	// keep track of number of barcodes, used later
	unordered_set<int> barcode_num;
	// hold all comma separated data to insert into hashmaps
	vector<vector<string>> row_info;

	string row;
	std::getline(barcode_file, row);  // The first row should be the header
	// Collect all comma separated rows and keep track of how many barcodes
	for (string row; std::getline(barcode_file, row);) {
		vector<string> row_vec = take_columns(3, row);
		row_info.push_back(row_vec);
		barcode_num.insert(std::stoi(row_vec[2]));
	}

	// Get total number of counted barcodes per sequence. ie for DEL this
	// can be 3 or 4
	int total_barcodes;
	for (const auto &num : barcode_num) {
		if (num > total_barcodes) {
			total_barcodes = num;
		}
	}

	// For each counted barcode in a sequence, create empty hashmaps and
	// hashsets
	stringmap empty_hashmap;
	stringset empty_hashset;
	for (int i = 0; i < total_barcodes; i++) {
		counted_barcodes_hash.push_back(empty_hashmap);
		counted_barcodes_seqs.push_back(empty_hashset);
	}

	// Insert info into vectors of hashmaps and hashsets while using the
	// index as the counted barcode number
	for (int i = 0; i < row_info.size(); i++) {
		vector<string> row_vec = row_info[i];
		int barcode_num = std::stoi(row_vec[2]) - 1;
		pair<string, string> barcode_conv{row_vec[0], row_vec[1]};
		counted_barcodes_hash[barcode_num].insert(barcode_conv);
		counted_barcodes_seqs[barcode_num].insert(row_vec[0]);
	}
}

/// A print method created to test building of the algorithm
void BarcodeConversion::print() {
	cout << "Sample barcode conversion" << endl;
	for (const auto &myPair : samples_barcode_hash) {
		std::cout << "Key:[" << myPair.first << "] Value:["
			  << myPair.second << "]\n";
	}
	cout << endl;

	cout << "Counted barcode conversion" << endl;
	for (int i = 0; i < counted_barcodes_hash.size(); i++) {
		cout << endl << "Barcode number" << i + 1 << endl << endl;
		stringmap barcode_hash = counted_barcodes_hash[i];
		for (const auto &myPair : barcode_hash) {
			std::cout << "Key:[" << myPair.first << "] Value:["
				  << myPair.second << "]\n";
		}
	}
}
