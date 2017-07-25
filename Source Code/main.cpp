#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <istream>
#include <string>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <tuple>
#include <sys/stat.h>
#include <random>
using namespace std;

//Forward declarations: compiler needs to know function prototype when function call is compiled.
vector<string> split(const string &s, char delim);
void extractFrameID(ifstream &in, string fileToBeRead);
void extractFrameIDAndWriteID(ifstream &in, ofstream &of);
bool isFloat(string myString);
int MapSadnessToFile(string inputFile, vector<vector<float>> scaleValues);
float CalculateInterpolationValues(vector<float> dataVector);
vector<vector<float>> AddEmotionToInputFrames(vector<vector<float>> emotionInterpolationVectorStorage, vector<vector<float>> userInterpolationVectorStorage , float scale);
inline bool doesFileExist(const string& name);
string SelectEmotionMocapFile(string emotionWalkCycleMocapFile);
float AssignEmotionIntensity(float intensity);

//using tuples to return multiple parameters of different types.
tuple<vector<vector<float>>, int> ReadFile(string fileToBeRead);
tuple<int, int> CalculateWalkCycles(int emotionalAnimationLength, int inputAnimationLength);


//Variable declarations: needed for the duration of the program.
vector<vector<float>> emotionInterpolationVectorStorage;
vector<vector<float>> userInterpolationVectorStorage;
vector<vector<float>> dataModifiersVectorStorage;
int emotionalAnimationLength;
int inputAnimationLength;
string emotionWalkCycleMocapFile = "105_sadWalkOneCycle.amc";
string inputMocapFile = "105_normalWalkShortened.amc";
int numberOfWalkCycles;
int partialCycleLength;
string inputOption;
bool mainMenuClosed = false;
bool programOpen = true;
float emotionIntensity = 0;

int main() {
	cout << "Welcome to the motion capture modification program!" << endl;
	cout << endl;

	while (programOpen) {
		while (!mainMenuClosed) {
			try {
				cout << endl;
				cout << "MAIN MENU \n" << endl;
				cout << "Would you like to:" << endl;
				cout << "1. Modify your own walking animation." << endl;
				cout << "2. Modify the default walking animation (105_normalWalkShortened.amc)(Recommended). \n" << endl;
				cin >> inputOption;
				if (inputOption == "1") {

					cout << "Please enter the walking animation you wish to modify (filename):" << endl;
					cin >> inputMocapFile;
					//check to see if the file in question exists.
					if (doesFileExist(inputMocapFile)) {
						cout << "File exists!" << endl;
						mainMenuClosed = true;
						emotionWalkCycleMocapFile = SelectEmotionMocapFile(emotionWalkCycleMocapFile);
					}
					else {
						cout << endl;
						cout << " File does not exist! \n" << endl;
					}
					cin.get();
				}
				else if (inputOption == "2") {
					//resetting the default file name, in case the user previously overwrites it with an incorrect filename.
					inputMocapFile = "105_normalWalkShortened.amc";
					mainMenuClosed = true;
					cout << "SELECTED: " << inputMocapFile << endl;
					emotionWalkCycleMocapFile = SelectEmotionMocapFile(emotionWalkCycleMocapFile);
				}
				else {
					cout << endl;
					throw out_of_range("Incorrect entry \n");
				}


			}
			catch (exception& ex)
			{
				cout << "ERROR: " << ex.what() << '\n';
			}
		}

		//retrieve the value that will be used to change the intensity of the emotion applied to the input animation.
		emotionIntensity = AssignEmotionIntensity(emotionIntensity);

		cout << "Press ENTER to calculate the modification" << endl;
		cin.get();
		cin.get();

		//reading the sad walk mocap data from the file and storing the needed output in variables.
		tie(emotionInterpolationVectorStorage, emotionalAnimationLength) = ReadFile(emotionWalkCycleMocapFile);
		//reading the input data  and storing the needed output in variables.
		tie(userInterpolationVectorStorage, inputAnimationLength) = ReadFile(inputMocapFile);
		//calculate the number of sad walk cycles (and the remainder) that can fit into the length of the input animation, and store the result in variables.
		tie(numberOfWalkCycles, partialCycleLength) = CalculateWalkCycles(emotionalAnimationLength, inputAnimationLength);
		//apply the emotion chosen to the input file
		dataModifiersVectorStorage = AddEmotionToInputFrames(emotionInterpolationVectorStorage, userInterpolationVectorStorage, emotionIntensity);

		cout << endl;
		cout << "READING COMPLETE \n" << endl;
		cout << "Number of emotional walk cycles that can fit into your animation: " << numberOfWalkCycles << endl;
		cout << "Leftover frames: " << partialCycleLength << endl;
		cout << endl;
		cout << "Press ENTER to map your modification of your input animation to an ouptut file." << endl;
		cin.get();

		//write the modified motion capture data to a file.
		MapSadnessToFile(inputMocapFile, dataModifiersVectorStorage);

		cout << "Would you like to: \n" << endl;
		cout << "1. Modify another animation ( Warning: previous output file will be overwritten)" << endl;
		cout << "2. Exit the program" << endl;
		cin >> inputOption;
		if (inputOption != "1") {
			programOpen = false;
		}
		else {
			mainMenuClosed = false;
		}

	}
	return 0;
}
inline bool doesFileExist(const string& name) {

	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}
string SelectEmotionMocapFile(string emotionWalkCycleMocapFile) {
	bool emotionMenuClosed = false;
	while (!emotionMenuClosed) {
		try {
			cout << endl;
			cout << "EMOTION MENU" << endl;
			cout << "Would you like to:" << endl;
			cout << "1. Add emotion to your animation using your own emotional walk cycle." << endl;
			cout << "2. Add sadness to your walking animation (105_sadWalkOneCycle.amc) (Recommended)." << endl;
			cout << "3. Add happiness to your walking animation (142_happyWalkOneCycle.amc)." << endl;
			cout << endl;
			cin >> inputOption;
			if (inputOption == "1") {

				cout << "Please enter the sad walk cycle you wish to use (filename + extension. e.g. 91_sadWalkOneCycle.amc):" << endl;
				cin >> emotionWalkCycleMocapFile;

				if (doesFileExist(emotionWalkCycleMocapFile)) {
					emotionMenuClosed = true;
					cout << endl;
					cout << "SELECTED: " << emotionWalkCycleMocapFile << endl;
					
					return emotionWalkCycleMocapFile;
				}
				else {
					cout << " File does not exist!" << endl;
					cout << endl;
				}
				cin.get();
			}
			else if (inputOption == "2") {
			//setting the program to read the sadness emotion.
				emotionWalkCycleMocapFile = "105_sadWalkOneCycle.amc";
				emotionMenuClosed = true;
				cout << endl;
				cout << "SELECTED: " << emotionWalkCycleMocapFile << endl;
				cout << endl;
				return emotionWalkCycleMocapFile;
			}
			else if (inputOption == "3") {
			//setting the program to read the happiness emotion.
				emotionWalkCycleMocapFile = "142_happyWalkOneCycle.amc";
				emotionMenuClosed = true;
				cout << endl;
				cout << "SELECTED: " << emotionWalkCycleMocapFile << endl;
				cout << endl;
				return emotionWalkCycleMocapFile;
			}
			else {
				throw out_of_range("Incorrect entry");
			}


		}
		catch (exception& ex)
		{
			cout << "ERROR: " << ex.what() << '\n';
		}
	}
	return 0;
}
float AssignEmotionIntensity(float intensity) {
	bool magnitudeMenuClosed = false;
	while (!magnitudeMenuClosed) {
		try {
			cout << endl;
			cout << "MAGNITUDE MENU" << endl;
			cout << "Would you like to:" << endl;
			cout << "1. Set a custom value for the intensity of the animation:" << endl;
			cout << "2. Use the default value for the intensity of the animation(Default intensity: 1) (Recommended)." << endl;
			cout << "3. Set a random value (between 0.1 and 2) for the intensity of the animation" << endl;
			cout << endl;
			cin >> inputOption;
			if (inputOption == "1") {

				cout << endl;
				cout << "Please enter the intensity value you wish to use:" << endl;
				cout << endl;
				cin >> intensity;

				if (intensity > 0) {
					magnitudeMenuClosed = true;
					cout << endl;
					cout << "INTENSITY SELECTED: " << intensity << endl;
					cout << endl;
					return intensity;
				}
				else {
					cout << " Invalid number!" << endl;
					cout << endl;
				}
				cin.get();
			}
			else if (inputOption == "2") {
				//resetting the default file name, in case the user previously overwrites it with an incorrect filename.
				intensity = 1;
				magnitudeMenuClosed = true;
				cout << endl;
				cout << "INTENSITY SELECTED: " << intensity << endl;
				cout << endl;
				return intensity;
			}
			else if (inputOption == "3") {
			//intensity = rand()
				float minValue = 0.1f;
				float maxValue = 2.0f;
				//used once to initialise a seed.
				random_device rd;
				//defines the random number engine used (Mersenne-Twister).
				mt19937 randomNumberEngine(rd());
				//defines a guaranteed unbiased range.
				uniform_real_distribution<float> range(minValue, maxValue);
				//set the intensity to a random number within the range.
				intensity = range(randomNumberEngine);

				magnitudeMenuClosed = true;
				cout << endl;
				cout << "INTENSITY SELECTED: " << intensity << endl;
				cout << endl;
				return intensity;
			}
			else {
				throw out_of_range("Incorrect entry");
			}


		}
		catch (exception& ex)
		{
			cout << "ERROR: " << ex.what() << '\n';
		}
	}
	return 0;
}
tuple<vector<vector<float>>, int> ReadFile(string fileToBeRead) {

	string firstLineInFile;
	string secondLineInFile;
	string thirdLineInFile;
	vector<float> interpolationStorage;
	vector<vector<float>> interpolationVectorStorage;
	int animationLength = 0;
	vector<vector<float>> vectorStorage;
	vector<vector<float>> dataTypeVectorStorage;
	vector<float> dataTypeVector;

	//Create a vector to hold all the mocap data values in float form.
	vector<float> floatTokens;
	//Create a vector to hold an entire frame (the data and the words).
	vector<string> oneFrame;
	//initialise a vector with an empty set of vectors.
	for (size_t i = 0; i < 62; i++) {
		dataTypeVectorStorage.push_back(dataTypeVector);
	}

	//Create an input file stream
	ifstream in(fileToBeRead, ios::in);
	//cases to handle the first 3 lines of the file (first 3 lines contain no data, but are necessary)
	//extracts each line into an object and writes them to the top of the output file
	getline(in, firstLineInFile);
	getline(in, secondLineInFile);
	getline(in, thirdLineInFile);

	//loop until the end of the file is reached
	while (in.eof() == 0) {

		//create a buffer to store each frame
		stringstream buffer;
		//write the frameID to the file.
		extractFrameID(in, fileToBeRead);
		//loop around the 29 lines in a frame, push each line into the vector.
		for (int i = 0; i < 29; i++) {
			string tempString;
			getline(in, tempString);

			//if the end of the file is reached (.empty() is used as the last line in the mocap file is always an empty line.
			if (tempString.empty()) {

				//storing all the vectors of data types into a vector.
				for (size_t i = 0; i < 62; i++) {
					vectorStorage.push_back(dataTypeVectorStorage[i]);
				}
				
				//loop for every data type (62 degrees of freedom (including XYZ position)).
				for (size_t i = 0; i < 3; i++) {
					//interpolate between each data value and store the result in a vector.
					for (size_t j = 0; j < vectorStorage[i].size(); j++)
					{
						interpolationStorage.push_back(vectorStorage[i][j]);
					}

					interpolationVectorStorage.push_back(interpolationStorage);
					interpolationStorage.clear();
				}
				for (size_t i = 3; i < vectorStorage.size(); i++) {
					//interpolate between each data value and store the result in a vector.
					interpolationStorage.push_back(vectorStorage[i][0]);
					for (size_t j = 1; j < vectorStorage[i].size(); j++)
					{
						interpolationStorage.push_back(vectorStorage[i][j] - vectorStorage[i][j - 1]);
					}

					interpolationVectorStorage.push_back(interpolationStorage);
					interpolationStorage.clear();
				}
				vectorStorage.clear();
				cout << "Reading of " << fileToBeRead << " completed" << endl;
				in.close();
				//output the resulting vector with all the interpolation values and output the length of the animation (number of frames).
				return make_tuple(interpolationVectorStorage, animationLength);
			}
			oneFrame.push_back(tempString);
		}

		//populate the buffer with the vector.
		copy(oneFrame.begin(), oneFrame.end(), ostream_iterator<string>(buffer, "\n"));

		//split the buffer up into tokens(objects) and store them into a vector 
		vector<string> mainTokenVector = split(buffer.str(), ' ');

		//defining  vectors.
		vector<float> floatTokenVector;
		vector<string> stringTokenVector;

		//loop to split up the token vector into strings and floats, and store them in vectors
		for (size_t i = 0; i < mainTokenVector.size(); i++) {
			//if the token is a string, put it in the string vector
			if (isFloat(mainTokenVector[i]) == 0) {
				stringTokenVector.push_back(mainTokenVector[i]);
			}
			//if the token is a float, put it in the float vector
			else if (isFloat(mainTokenVector[i]) == 1) {
				floatTokenVector.push_back(stof(mainTokenVector[i]));
			}
		}

		//pushing all data values to their responding vectors in order to interpolate and scale between them later.
		for (size_t i = 0; i < floatTokenVector.size(); i++) {
			dataTypeVectorStorage[i].push_back(floatTokenVector[i]);
		}

		//clear the vectors to prepare them for the next frame.
		oneFrame.clear();
		mainTokenVector.clear();
		stringTokenVector.clear();
		floatTokenVector.clear();
		//increment animationLength, used to symbolise the number of frames in the animation.
		animationLength++;
	}
	//close the file currently associated with the object, disassociating it from the stream.
	in.close();
	return make_tuple(interpolationVectorStorage,0);
}
tuple<int, int> CalculateWalkCycles(int emotionalAnimationLength, int inputAnimationLength) {
	float numberOfWalkCycles;
	int partialCycleLength;

	float divisionResult;
	float numCycles;

	divisionResult = (float)(inputAnimationLength / emotionalAnimationLength);
	//get the number of times the emotion walk cycle can fit into the input animatio.
	numCycles = modf(divisionResult, &numberOfWalkCycles);

	//grabs the remaining number of frames after the last full walk cycle.
	partialCycleLength = inputAnimationLength % emotionalAnimationLength;

	return make_tuple(numberOfWalkCycles, partialCycleLength);
}
vector<vector<float>> AddEmotionToInputFrames(vector<vector<float>> emotionInterpolationVectorStorage, vector<vector<float>> userInterpolationVectorStorage, float scale) {
#pragma region Initialising dataModifiersVectorStorage 
	vector<float> dataModifiersStorage;
	vector<vector<float>> dataModifiersVectorStorage;
	int numberOfFrames = 0;

	//loop for every data type/degree of freedom (lowerBackX lowerBackY lowerBack z etc).
	for (size_t i = 0; i < emotionInterpolationVectorStorage.size(); i++) {
		numberOfFrames = 0;
		//loop over the data in the walk cycles.
		for (int j = 0; j < numberOfWalkCycles; j++) {

			for (size_t k = 0; k < emotionInterpolationVectorStorage[i].size(); k++)
			{
				dataModifiersStorage.push_back((userInterpolationVectorStorage[i][k + (numberOfFrames)] - emotionInterpolationVectorStorage[i][k]));
			}
			numberOfFrames = (numberOfFrames + emotionInterpolationVectorStorage[i].size());
		}
		//looping over the leftover frames that could not fill a walk cycle.
		for (int l = 0; l < partialCycleLength; l++)
		{
			dataModifiersStorage.push_back((userInterpolationVectorStorage[i][l + (numberOfFrames)] - emotionInterpolationVectorStorage[i][l]));
		}
		dataModifiersVectorStorage.push_back(dataModifiersStorage);
		dataModifiersStorage.clear();
	}
#pragma endregion Initialising vector with values.
#pragma region Modifying Rotational Values
	//separated from the Positional values as the character's position increases linearly whereas the rotation repeats.
	float currentRotation = 0;
	vector<float> valuesVector;
	//loop for the root X Y and Z positions of the character.
	for (size_t i = 3; i <emotionInterpolationVectorStorage.size(); i++) {
		dataModifiersVectorStorage[i][0] = emotionInterpolationVectorStorage[i][0];
		//set the character's current position (on the specified axis) to the first value from the sad animation.
		currentRotation = emotionInterpolationVectorStorage[i][0];
		numberOfFrames = 0;
		//for the number of walk cycles
		for (int j = 0; j < numberOfWalkCycles; j++) {
			//for the number of sad frames
			for (size_t k = 1; k < emotionInterpolationVectorStorage[0].size(); k++) {

				valuesVector.push_back(emotionInterpolationVectorStorage[i][k]);

				currentRotation = currentRotation + (valuesVector[(k + numberOfFrames - 1)] * scale);

				dataModifiersVectorStorage[i][k + numberOfFrames] = currentRotation;
			}
			numberOfFrames = (numberOfFrames + emotionInterpolationVectorStorage[i].size() - 1);
			currentRotation = emotionInterpolationVectorStorage[i][0];
		}
		//looping over the leftover frames that could not fill a walk cycle.
		for (int l = 1; l < partialCycleLength; l++)
		{
			valuesVector.push_back(emotionInterpolationVectorStorage[i][l]);

			currentRotation = currentRotation + (valuesVector[(l + (numberOfFrames)) - 1] * scale);

			dataModifiersVectorStorage[i][l + numberOfFrames] = currentRotation;
		}
		//case to handle the last couple of positions in the animation not being correctly set.
		for (int m = 0; m < numberOfWalkCycles; m++) {
			dataModifiersVectorStorage[i][dataModifiersVectorStorage[i].size() - (m + 1)] = dataModifiersVectorStorage[i][dataModifiersVectorStorage[i].size() - (numberOfWalkCycles + 1)];
		}

		valuesVector.clear();
		currentRotation = 0;
	}
#pragma endregion 
#pragma region Modifying Positional values (RootXYZ)
	float currentPosition = 0;
	//clear the vector in preparation for using it for the character's positional values.
	valuesVector.clear();
	//loop for the root X Y and Z positions of the character.
	for (size_t i = 0; i < 3; i++) {
		dataModifiersVectorStorage[i][0] = emotionInterpolationVectorStorage[i][0];
		//set the character's current position (on the specified axis) to the first value from the sad animation.
		currentPosition = emotionInterpolationVectorStorage[i][0];
		numberOfFrames = 0;
		//for the number of walk cycles
		for (int j = 0; j < numberOfWalkCycles; j++) {
			//for the number of sad frames
			for (size_t k = 1; k < emotionInterpolationVectorStorage[0].size(); k++) {
				//interpolate between the current and previous position to find the distance between the positions.
				valuesVector.push_back(emotionInterpolationVectorStorage[i][k] - emotionInterpolationVectorStorage[i][k - 1]);
				//update the character's position with the interpolation value. Here the value can be scaled to change the speed and distance travelled.
				currentPosition = currentPosition + (valuesVector[(k + numberOfFrames - 1)] * scale);
				//put the position data in a vector in preparation to be written to the output file.
				dataModifiersVectorStorage[i][k + numberOfFrames] = currentPosition;
			}
			numberOfFrames = (numberOfFrames + emotionInterpolationVectorStorage[i].size() - 1);
		}
		//looping over the leftover frames that could not fill a walk cycle.
		for (int l = 1; l < partialCycleLength; l++)
		{
			//interpolate between the current and previous position to find the distance between the positions.
			valuesVector.push_back(emotionInterpolationVectorStorage[i][l] - emotionInterpolationVectorStorage[i][l - 1]);
			//update the character's position with the interpolation value. Here the value can be scaled to change the speed and distance travelled.
			currentPosition = currentPosition + (valuesVector[(l + numberOfFrames) - 1] * scale);
			//put the position data in a vector in preparation to be written to the output file.
			dataModifiersVectorStorage[i][l + numberOfFrames] = currentPosition;
		}
		//case to handle the last couple of positions in the animation not being correctly set.
		for (int m = 0; m < numberOfWalkCycles; m++) {
			dataModifiersVectorStorage[i][dataModifiersVectorStorage[i].size() - (m + 1)] = dataModifiersVectorStorage[i][dataModifiersVectorStorage[i].size() - (numberOfWalkCycles + 1)];
		}

		valuesVector.clear();
		currentPosition = 0;
	}
#pragma endregion
	return dataModifiersVectorStorage;
}
int MapSadnessToFile(string inputFile, vector<vector<float>> modificationValues) {
	ifstream in(inputFile, ios::in);
	string firstLineInFile;
	string secondLineInFile;
	string thirdLineInFile;

	//Create a vector to hold all the mocap data values in float form.
	vector<float> floatTokens;
	//Create a vector to hold an entire frame (the data and the words).
	vector<string> oneFrame;
	//Create an interator used to navigate the modificationValues vector
	int scaleIterator = 0;
	//split the input file into substrings. To be used for the outputFileName;
	string fileName = inputFile.substr(0, (inputFile.length() - 4));
	string fileExtension = inputFile.substr((inputFile.length() - 4), inputFile.length());
	string modifiedOutputFileName = fileName + "Modified" + fileExtension;

	//define an output stream object
	ofstream outputTestFile;
	//creating the output file
	outputTestFile.open(modifiedOutputFileName);
	//cases to handle the first 3 lines of the file (first 3 lines contain no data, but are necessary).
	//extract each line into an object.  
	getline(in, firstLineInFile);
	getline(in, secondLineInFile);
	getline(in, thirdLineInFile);
	//write them to the top of the output file.
	outputTestFile << firstLineInFile << endl;
	outputTestFile << secondLineInFile << endl;
	outputTestFile << thirdLineInFile << endl;

	while (in.eof() == 0) {

		//create a buffer to store each frame.
		stringstream buffer;
		//write the frameID to the file.
		extractFrameIDAndWriteID(in, outputTestFile);
		//loop around the 29 lines in a frame, push each line into the vector.
		//there will always be 29 lines (after the frameID) for the AMC format.
		for (int i = 0; i < 29; i++) {
			string tempString;
			getline(in, tempString);

			//if the end of the file is reached, exit.  (.empty() is used as the last line in the mocap file is always an empty line.
			if (tempString.empty()) {
				cout << endl;
				cout << "MODIFICATION COMPLETE" << endl;
				cout << endl;
				cout << "The output file is called: " + modifiedOutputFileName << endl;
				cout << "And is located in the ModifyMocapData folder \n" << endl;

				return 0;
			}
			oneFrame.push_back(tempString);

		}

		copy(oneFrame.begin(), oneFrame.end(), ostream_iterator<string>(buffer, "\n"));

		//split the buffer up into tokens(objects) and store them into a vector 
		vector<string> mainTokenVector = split(buffer.str(), ' ');

		//defining  separate vectors for strings and floats.
		vector<float> floatTokenVector;
		vector<string> stringTokenVector;

		//loop to split up the token vector into strings and floats, and store them in vectors
		for (size_t i = 0; i < mainTokenVector.size(); i++) {
			//if the token is a string, put it in the string vector
			if (isFloat(mainTokenVector[i]) == 0) {
				stringTokenVector.push_back(mainTokenVector[i]);
			}
			//if the token is a float, put it in the float vector
			else if (isFloat(mainTokenVector[i]) == 1) {
				floatTokenVector.push_back(stof(mainTokenVector[i]));
			}

		}
		float globalScale = 0.5f;
		//mapping the new character root positions to the variables.
		floatTokenVector[0] = modificationValues[0][scaleIterator];
		floatTokenVector[1] = modificationValues[1][scaleIterator];
		floatTokenVector[2] = modificationValues[2][scaleIterator];

		//skipping over the first 3 values because they relate to the character's position, which is linear and not repeating.
		for (size_t i = 3; i < floatTokenVector.size(); i++) {
			//check if the result of modification is a real number. If it isn't, it will break the animation.
			if ((isnan(floatTokenVector[i] = modificationValues[i][(scaleIterator)])) ) {
				
				if ((isnan(floatTokenVector[i] = modificationValues[i][(scaleIterator-1)])))
				{
				//if the result behind it isn't a real number either, then don't change the data value.
					floatTokenVector[i] = floatTokenVector[1] * 1;
				}
				else {
				// if the previous result is a real number, then use it to scale the current data value.
				//This fixes a stuttering issue caused by numbers too small to divide with.
					floatTokenVector[i] =  modificationValues[i][((scaleIterator - 1 ))] ;
				}
			}
			else {
				//if the result is a real number, then use it to scale the current data value.
				floatTokenVector[i] =  modificationValues[i][(scaleIterator)];
			}
		}
		scaleIterator++;

#pragma region Frame Structure
		//recreating the structure of a single mocap frame in preparation for it to be outputted to a file.
		string rootData = stringTokenVector[0] + " " + to_string(floatTokenVector[0]) + " " + to_string(floatTokenVector[1]) + " " + to_string(floatTokenVector[2])
			+ " " + to_string(floatTokenVector[3]) + " " + to_string(floatTokenVector[4]) + " " + to_string(floatTokenVector[5]);
		string lowerbackData = stringTokenVector[1] + " " + to_string(floatTokenVector[6]) + " " + to_string(floatTokenVector[7]) + " " + to_string(floatTokenVector[8]);
		string upperBackData = stringTokenVector[2] + " " + to_string(floatTokenVector[9]) + " " + to_string(floatTokenVector[10]) + " " + to_string(floatTokenVector[11]);
		string thoraxData = stringTokenVector[3] + " " + to_string(floatTokenVector[12]) + " " + to_string(floatTokenVector[13]) + " " + to_string(floatTokenVector[14]);
		string lowerneckData = stringTokenVector[4] + " " + to_string(floatTokenVector[15]) + " " + to_string(floatTokenVector[16]) + " " + to_string(floatTokenVector[17]);
		string upperneckData = stringTokenVector[5] + " " + to_string(floatTokenVector[18]) + " " + to_string(floatTokenVector[19]) + " " + to_string(floatTokenVector[20]);
		string headData = stringTokenVector[6] + " " + to_string(floatTokenVector[21]) + " " + to_string(floatTokenVector[22]) + " " + to_string(floatTokenVector[23]);
		string rclavicleData = stringTokenVector[7] + " " + to_string(floatTokenVector[24]) + " " + to_string(floatTokenVector[25]);
		string rhumerusData = stringTokenVector[8] + " " + to_string(floatTokenVector[26]) + " " + to_string(floatTokenVector[27]) + " " + to_string(floatTokenVector[28]);
		string rradiusData = stringTokenVector[9] + " " + to_string(floatTokenVector[29]);
		string rwristData = stringTokenVector[10] + " " + to_string(floatTokenVector[30]);
		string rhandData = stringTokenVector[11] + " " + to_string(floatTokenVector[31]) + " " + to_string(floatTokenVector[32]);
		string rfingersData = stringTokenVector[12] + " " + to_string(floatTokenVector[33]);
		string rthumbData = stringTokenVector[13] + " " + to_string(floatTokenVector[34]) + " " + to_string(floatTokenVector[35]);
		string lclavicleData = stringTokenVector[14] + " " + to_string(floatTokenVector[36]) + " " + to_string(floatTokenVector[37]);
		string lhumerusData = stringTokenVector[15] + " " + to_string(floatTokenVector[38]) + " " + to_string(floatTokenVector[39]) + " " + to_string(floatTokenVector[40]);
		string lradiusData = stringTokenVector[16] + " " + to_string(floatTokenVector[41]);
		string lwristData = stringTokenVector[17] + " " + to_string(floatTokenVector[42]);
		string lhandData = stringTokenVector[18] + " " + to_string(floatTokenVector[43]) + " " + to_string(floatTokenVector[44]);
		string lfingersData = stringTokenVector[19] + " " + to_string(floatTokenVector[45]);
		string lthumbData = stringTokenVector[20] + " " + to_string(floatTokenVector[46]) + " " + to_string(floatTokenVector[47]);
		string rfemurData = stringTokenVector[21] + " " + to_string(floatTokenVector[48]) + " " + to_string(floatTokenVector[49]) + " " + to_string(floatTokenVector[50]);
		string rtibiaData = stringTokenVector[22] + " " + to_string(floatTokenVector[51]);
		string rfootData = stringTokenVector[23] + " " + to_string(floatTokenVector[52]) + " " + to_string(floatTokenVector[53]);
		string rtoesData = stringTokenVector[24] + " " + to_string(floatTokenVector[54]);
		string lfemurData = stringTokenVector[25] + " " + to_string(floatTokenVector[55]) + " " + to_string(floatTokenVector[56]) + " " + to_string(floatTokenVector[57]);
		string ltibiaData = stringTokenVector[26] + " " + to_string(floatTokenVector[58]);
		string lfootData = stringTokenVector[27] + " " + to_string(floatTokenVector[59]) + " " + to_string(floatTokenVector[60]);
		string ltoesData = stringTokenVector[28] + " " + to_string(floatTokenVector[61]);
#pragma endregion recreating the file frame structure
		
		//clearing the vectors in preparation for the next frame.
		oneFrame.clear();
		mainTokenVector.clear();
		stringTokenVector.clear();
		floatTokenVector.clear();

		//write the contents of the strings (that form a single frame) to the output file.
		outputTestFile << rootData << endl;
		outputTestFile << lowerbackData << endl;
		outputTestFile << upperBackData << endl;
		outputTestFile << thoraxData << endl;
		outputTestFile << lowerneckData << endl;
		outputTestFile << upperneckData << endl;
		outputTestFile << headData << endl;
		outputTestFile << rclavicleData << endl;
		outputTestFile << rhumerusData << endl;
		outputTestFile << rradiusData << endl;
		outputTestFile << rwristData << endl;
		outputTestFile << rhandData << endl;
		outputTestFile << rfingersData << endl;
		outputTestFile << rthumbData << endl;
		outputTestFile << lclavicleData << endl;
		outputTestFile << lhumerusData << endl;
		outputTestFile << lradiusData << endl;
		outputTestFile << lwristData << endl;
		outputTestFile << lhandData << endl;
		outputTestFile << lfingersData << endl;
		outputTestFile << lthumbData << endl;
		outputTestFile << rfemurData << endl;
		outputTestFile << rtibiaData << endl;
		outputTestFile << rfootData << endl;
		outputTestFile << rtoesData << endl;
		outputTestFile << lfemurData << endl;
		outputTestFile << ltibiaData << endl;
		outputTestFile << lfootData << endl;
		outputTestFile << ltoesData << endl;
	}

	in.close();
	outputTestFile.close();
	return 0;
}
bool isFloat(string myString) {
	istringstream iss(myString);
	float f;
	// noskipws considers leading whitespace invalid.
	iss >> noskipws >> f; 
	// Check the entire string was consumed, and if either failbit or badbit is set.
	return iss.eof() && !iss.fail();
}
vector<string> split(const string &s, char delim) {
	stringstream ss(s);
	string item;
	vector<string> tokens;

	while (ss >> item) {
		tokens.push_back(item);	
	}
	return tokens;
}
void extractFrameID(ifstream &in, string fileToBeRead) {
	string frameID;
	//retrieve the first line of the frame, which contains the frame ID.
	getline(in, frameID);
	if (frameID.empty()) {
		return;
	}
	cout << "Reading " + fileToBeRead + " Frame " + frameID << endl;
	frameID.clear();
}
void extractFrameIDAndWriteID(ifstream &in, ofstream &of) {
	string frameID;
		
	getline(in, frameID);
	if (frameID.empty()) {
		return;
	}
	//write the frameID to the output file.
	of << frameID << endl;
	cout << "Modifying Frame " + frameID << endl;
	frameID.clear();
}

