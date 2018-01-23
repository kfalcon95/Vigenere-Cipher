/*
Test Cipher:
VVHQWVVRHMUSGJGTHKIHTSSEJGHLSFCBGVWCRLRYQTFSVGAHWKCUHWAUGLQHNSLRLJSHBLTSPISPRDXLJSVEEGHLQWKASSKUWEPWQTWVSPGOELKCQYFNSVWLJSNIQKGNRGYBWLWGOVIOKHKAZKQKXZGYHCECMEIUJOQKWFWVEFQHKIJRCLRLKBIENQFRJLJSDHGRHLSFQTWLAUQRHWDMWLGUSGIKKFLRYVCWVSPGPMLKASSJVOQXEGGVEYGGZMLJCXXLJSVPAIVWIKVRDRYGFRJLJSLVEGGVEYGGEIAPUUISFPBTGNWWMUCZRVTWGLRWUGUMNCZVILE
*/
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

std::string decrypt(std::string, std::string);
std::string formatKey(std::string, std::string);

int main()
{
	char usr_inp, alpha[26] = { 'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z' };
	int i, j, k, count, key_len, occurencesSum = 0, len_guess = 1, highestIndex, freq[50] = { 0 };
	std::string c_txt, p_txt, final_key;
	std::vector<int> sums, let_freqs(26, 0);
	std::vector<std::pair<int, int>> finals;
	std::vector<double> W(26, 0.0), WAj(26, 0.0), rot(26);
	std::vector<std::string> shift;
	double highest, dotproduct = 0.0;
	double actual[26] = { 0.082, 0.015, 0.028, 0.043, 0.127,		// Actual frequency of English letters
		0.022, 0.020, 0.061, 0.070, 0.002,
		0.008, 0.040, 0.024, 0.067, 0.075,
		0.019, 0.001, 0.06, 0.063, 0.091,
		0.028, 0.01, 0.023, 0.001,0.020, 0.001 };

	std::cout << "Enter the Cipher Text:\n";
	std::cin >> c_txt;

	shift.push_back(c_txt);

	for (i = 0; i < c_txt.length(); i++)			// Counting how many times a letter appeared in the cipher text
		let_freqs[tolower(c_txt[i]) - 'a']++;

	for (i = 1; i < c_txt.length(); i++)
		shift.push_back(c_txt.substr(0, c_txt.length() - i));		// Shift each letter (by 1 to the right) of the cipher until all letters are shifted

	for (i = 1; i < shift.size(); i++)
	{
		count = 0;
		for (j = shift[i].size() - 1, k = shift[0].size() - 1; j >= 0; j--, k--)	// Compare each letter in original row with each following row, comparing each letter in each column
			if (shift[i][j] == shift[0][k])											// If two letters are the same, increase freq count
				count++;
		sums.push_back(count);
	}

	std::cout << "Number of repeating letters after each shift:\n";		// Print the number of repeating letters in columns found in each comparison between row 1 and row i
	for (i = 0; i < 50; i++)
		std::cout << "S" << i + 1 << ":" << sums[i] << " ";

	// Key guessing algorithm

	for (i = 0; i < sums.size(); i++)
		if (sums[i] >= c_txt.size() / 30)
			finals.push_back(std::make_pair(sums[i], i));

	for (i = 1; i < finals.size(); i++)
		freq[finals[i].second - finals[i - 1].second]++;

	for (i = 1; i < 50; i++)
		if (freq[i] > len_guess)
			len_guess = i;

	std::cout << "\n\nGuessed key length: " << len_guess;

	std::cout << "\nConfirm guessed key length? Enter 'y' or 'n': ";
	std::cin >> usr_inp;

	if (tolower(usr_inp) == 'y')
		key_len = len_guess;
	else if (tolower(usr_inp) == 'n')
	{
		std::cout << "Please enter the key length: ";
		std::cin >> key_len;
	}
	else
	{
		std::cout << "Invalid input. Program closing, press any key and press enter to continue.\n";
		std::cin >> usr_inp;
		return 0;
	}

	for (k = 0; k < key_len; k++)
	{
		for (i = 0 + k; i < c_txt.length(); i += key_len)	// Check how many times a letter occurs in 1st, 6th 11th,... then 2nd, 7th...3rd, 8th...etc. position.
			let_freqs[tolower(c_txt[i]) - 'a']++;

		rot.assign(actual, actual + sizeof(actual));

		for (i = 0; i < let_freqs.size(); i++)
			occurencesSum += let_freqs[i];

		for (i = 0; i < let_freqs.size(); i++)
			W[i] = (double)let_freqs[i] / occurencesSum;	// Array W = frequencies; W approximates A subscript i where i is the shift caused by the first element of the key


		for (j = 0; j < let_freqs.size(); j++)		// If we compute W * A subscript j for 0 <= j <= 25, max value should occur when j = i 
		{
			for (int i = 0; i < let_freqs.size(); i++)
				dotproduct = dotproduct + W[i] * rot[(i + ((26 - j) % 26)) % 26];		// Rotate right by j
			WAj[j] = dotproduct;		// Compute W * A subscript J from 0-25
			dotproduct = 0.0;
		}

		highest = WAj[0];
		highestIndex = 0;

		for (j = 1; j < let_freqs.size(); j++)
		{
			if (highest - WAj[j] < 0.0000001)	// Find the max of WAj
			{
				highest = WAj[j];
				highestIndex = j;
			}
		}
		final_key += alpha[highestIndex];

		// Resetting letter frequencies values back to 0
		std::fill(let_freqs.begin(), let_freqs.end(), 0);
	}

	std::cout << "\nKey:  " << final_key << std::endl;

	// Formatting and decrypting the cipher text to plain text

	final_key = formatKey(c_txt, final_key);
	p_txt = decrypt(c_txt, final_key);

	std::cout << p_txt;

	std::cout << "\n\nProgram closing, press any key and press enter to continue." << std::endl;
	std::cin >> usr_inp;
	return 0;
}

// Apply key to every letter of cipher
std::string formatKey(std::string str, std::string key)
{
	int len = str.size();

	for (int i = 0; key.size() < len; i++)
	{
		if (len == i)
			i = 0;
		key.push_back(key[i]);
	}
	return key;
}

std::string decrypt(std::string cipherTxt, std::string key)
{
	std::string plainTxt;
	int x, len = cipherTxt.size();

	for (int i = 0; i < len; i++)
	{
		x = (cipherTxt[i] - key[i] + 26) % 26;
		x += 'a';
		plainTxt.push_back(x);
	}
	return plainTxt;
}