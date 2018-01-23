/*
Test Cipher:
VVHQWVVRHMUSGJGTHKIHTSSEJCHLSFCBGVWCRLRYQTFSVGAHWKCUHWAUGLQHNSLRLJSHBLTSPISPRDXLJSVEEGHLQWKASSKUWEPWQTWVSPGOELKCQYFNSVWLJSNIQKGNRGYBWLWGOVIOKHKAZKQKXZGYHCECMEIUJOQKWFWVEFQHKIJRCLRLKBIENQFRJLJSDHGRHLSFQTWLAUQRHWDMWLGUSGIKKFLRYVCWVSPGPMLKASSJVOQXEGGVEYGGZMLJCXXLJSVPAIVWIKVRDRYGFRJLJSLVEGGVEYGGEIAPUUISFPBTGNWWMUCZRVTWGLRWUGUMNCZVILE
*/
#include <iostream>
#include <vector>
#include <string>

std::string decrypt(std::string, std::string);
std::string formatKey(std::string, std::string);

int main()
{
	char usr_inp, mostFreqLetter;
	int len_guess, count, freq[50] = { 0 }, i, j, k, key_len, index, highest;
	std::string c_txt, final_key, p_txt;
	std::vector<int> letter_freqs(26, 0), sums;
	std::vector<std::string> shift;
	std::vector<std::pair<int, int>> finals;

	std::cout << "Enter the Cipher Text:\n";
	std::cin >> c_txt;

	shift.push_back(c_txt);

	for (i = 1; i < c_txt.length(); i++)
		shift.push_back(c_txt.substr(0, c_txt.length() - i));		// Shift each letter (by 1 to the right) of the cipher until all letters are shifted

	for (i = 1; i < shift.size(); i++)
	{
		count = 0;
		for (j = shift[i].size() - 1, k = shift[0].size() - 1; j >= 0; j--, k--)	// Compare each letter in original row with each following row, comparing each letter in each column
			if (shift[i][j] == shift[0][k])							// If two letters are the same, increase freq count
				count++;
		sums.push_back(count);
	}

	std::cout << "Number of repeating letters after each shift:\n";		// Print the number of repeating letters in columns found in each comparison between original row and row i
	for (i = 0; i < 50; i++)
		std::cout << "S" << i + 1 << ":" << sums[i] << " ";

	// Guess key length based on coincidences and displacement

	for (i = 0; i < sums.size(); i++)
		if (sums[i] >= c_txt.size() / 30)
			finals.push_back(std::make_pair(sums[i], i));

	for (i = 1; i < finals.size(); i++)
		freq[finals[i].second - finals[i - 1].second]++;

	len_guess = 1;
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

	count = 0;

	for (j = 0; j < key_len; j++)
	{
		for (i = j; i < c_txt.length(); i += key_len)
			letter_freqs[tolower(c_txt[i]) - 'a']++;

		highest = letter_freqs[0];		// Start with 'A' as most frequent letter
		mostFreqLetter = 65;

		for (i = 1; i < letter_freqs.size(); i++)
		{
			if (highest < letter_freqs[i])	// Compare current most frequent letter against next letter
			{
				index = i;						// Save current letter position (in case user wants to discard)
				highest = letter_freqs[i];	// Update most frequent letter
				mostFreqLetter = (i + 65) - 'E' + 65;	// Convert to ASCII representation, then subtract 'E' (assumes most frequent cipher letter = most frequent English letter)
														// to get position in alphabet of key character, then add 65 
														// to convert alphabet position to ASCII representation
			}
		}

		std::cout << "\nHighest Frequency Letter #" << j + 1 << ": " << mostFreqLetter << std::endl;

		std::cout << "Confirm this letter for the key? Enter 'y' or 'n': ";
		std::cin >> usr_inp;

		if (tolower(usr_inp) == 'y')
			final_key += mostFreqLetter;
		else if (tolower(usr_inp) == 'n')		// Repeat previous algorithm for next most frequent letter
		{
			while (tolower(usr_inp) == 'n')
			{
				letter_freqs[index] = -1;		// Discarding letter that user responded 'n' to
				std::cout << "\nTrying the next frequency letter...\n";
				highest = letter_freqs[0];

				for (int i = 1; i < letter_freqs.size(); i++)
				{
					if (highest < letter_freqs[i])
					{
						index = i;
						highest = letter_freqs[i];
						mostFreqLetter = (i + 65) - 'E' + 65;
					}
				}

				std::cout << "Highest Frequency Letter #" << j + 1 << ": " << mostFreqLetter << std::endl;

				std::cout << "Confirm this letter for the key? Enter 'y' or 'n': ";
				std::cin >> usr_inp;
			}
			final_key += mostFreqLetter;
		}
		else
		{
			std::cout << "\nInvalid choice. Assuming that the letter is OK.\n";
			final_key += mostFreqLetter;
		}

		// Resetting letter frequencies values back to 0
		std::fill(letter_freqs.begin(), letter_freqs.end(), 0);
	}

	for (int i = 0; i < final_key.length(); i++)
		final_key[i] = toupper(final_key[i]);

	std::cout << "Key: " << final_key << std::endl;

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