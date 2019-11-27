/*
 * stream_ciphers.cpp
 *
 *  Created on: Nov. 2, 2019
 *      Author: Huacheng Qin
 */

//================================================================================================

#include <iostream>
#include <cctype>

//================================================================================================

//Function declarations
#ifndef MARMOSET_TESTING
int main();
#endif
char *encode(char *plaintext, unsigned long key);
char *decode( char *ciphertext, unsigned long key );
void algorithm(char *input, char output[], unsigned long key, std::size_t capacity);
void ascii_armour(char *input, char output[], std::size_t capacity);
void armour_off(char *input, char output[], std::size_t capacity);
std::size_t find_size(char *arr);

//================================================================================================

#ifndef MARMOSET_TESTING
int main() {
	unsigned long key { 51323 };
	char arr[] { "hello ECE 150 class" };
	char arr2[] { "A Elbereth Gilthoniel\nsilivren penna miriel\n"
			"o menel aglar elenath!\nNa-chaered palan-diriel\n"
			"o galadhremmin ennorath,\nFanuilos, le linnathon\n"
			"nef aear, si nef aearon!" };

	std::cout << arr << std::endl;
	char *ciphertext{encode(arr, key)};
	char *plaintext{decode(ciphertext, key)};

	std::cout << ciphertext << std::endl;
	std::cout << plaintext << std::endl;

	delete[] ciphertext;
	delete[] plaintext;

	std::cout << arr2 << std::endl;
	ciphertext = encode(arr2, key);
	plaintext = decode(ciphertext, key);

	std::cout << ciphertext << std::endl;
	std::cout << plaintext << std::endl;

	delete[] ciphertext;
	delete[] plaintext;

	return 0;
}//main();
#endif

//================================================================================================

//encode()
//	This function takes in a string of text and returns the text encrypted
//char *plaintext - string that will be encrypted
//unsigned long key - key used for encryption
char *encode(char *plaintext, unsigned long key) {
	//Increase size to the next multiple of 4 if size is not a multiple of 4
	std::size_t size {find_size(plaintext)};//Size of plaintext
	std::size_t old_size{size};
	while (size % 4 > 0){
		size++;
	}
	char midpoint[size];		//Encrypted data containing unprintable characters
	char *end_result{new char [(size / 4) * 5 + 1]};	//Fully encrypted printable data

	//Add NULL characters to match size
	for (std::size_t x{0}; x < size; x++){
		if (x < old_size){
			midpoint[x] = plaintext[x];
		}
		else{
			midpoint[x] = '\0';
		}
	}

	//Encrypt plaintext
	algorithm(midpoint, midpoint, key, size);
	//Convert encyrpted text to printable values
	ascii_armour(midpoint, end_result, size);

	return end_result;
}//encode();

//================================================================================================

//decode()
//	This function takes in a string of entrypted text and returns the text decrypted
//char *ciphertext - encrypted string
//unsigned long key - key used for encryption
char *decode( char *ciphertext, unsigned long key){
	std::size_t size { find_size(ciphertext) };//Size of plaintext
	char midpoint[(size / 5) * 4 + 1];			//Encrypted data containing unprintable characters
	char *end_result{new char [(size / 5) * 4 + 1]};		//Fully encrypted printable data

	//Remove ascii armour
	armour_off(ciphertext, midpoint, size);
	//Decrypt text
	algorithm(midpoint, end_result, key, (size / 5) * 4 + 1);

	//Add NULL character to the end of the array
	end_result[(size / 5) * 4] = '\0';

	return end_result;
}//decode();

//================================================================================================

//algorithm()
//	This function applies exlusive or to each character with a psuedo random character
//	generated with the given key
//char *input - string before the algorithm is applied
//char output[] - string after the algorithm is applied
//unsigned long key - key used to generate numbers
//std::size_t capacity - size of input string
void algorithm(char *input, char output[], unsigned long key, std::size_t capacity){
	unsigned char S[256];			//Array containing values 0 - 255
	unsigned char i { 0 }, j { 0 };	//Positional counters modulo 256;
	unsigned char temp;				//Placeholder for switching two variables

	//Assign 0 - 255 to S
	for (int x {0}; x < 256; ++x){
		S[x] = x;
	}
	//Psuedo randomize S with given key
	for (int x { 0 }; x < 256; ++x) {
		int k = i % 64;
		j += S[i] + ((key >> k) & 1);
		temp = S[i];
		S[i] = S[j];
		S[j] = temp;
		++i;
	}

	for (int x{0}; (unsigned)x < capacity; x++){
			std::cout << static_cast<int>(input[x]) << " ";
	}
	std::cout << std::endl;

	//Encrypt plaintext with S
	for (std::size_t x { 0 }; x < capacity; ++x) {
		i++;
		j += S[i];
		temp = S[i];
		S[i] = S[j];
		S[j] = temp;
		unsigned char R = S[i] + S[j];
		R = S[R];
		output[x] = R ^ input[x];
	}
}//algorithm();

//================================================================================================

//ascii_armour()
//	This function takes the input and converts it to printable char values
//	4 bytes at a time.
//char *input - the array to be converted
//char output[] - the resulting array
//std::size_t capacity - size of input[]
void ascii_armour(char *input, char output[], std::size_t capacity) {
	//Loop through through the input array in multiples of 4
	for (std::size_t count1 { 0 }; count1 < capacity / 4; ++count1) {
		//Add the value of the four bytes, read it as if it was an unsigned int
		unsigned int four_bytes { 0 };
		for (int count2 { 0 }; count2 < 4; ++count2) {
			four_bytes *= 256;
			four_bytes += static_cast<unsigned char>(input[count1 * 4 + count2]);
		}
		//Modify the output using the value of the four bytes modulo 85
		for (int count2 { 4 }; count2 >= 0; --count2) {
			output[count1 * 5 + count2] = '!' + (four_bytes % 85);
			four_bytes /= 85;
		}
	}//for (multiples of 4 through input)

	//Add NULL character to end output array
	output[(capacity / 4) * 5] = '\0';
}//ascii_armour();

//================================================================================================

//armour_off()
//	This function undoes the ascii armour applied using ascii_armour();
//char *input - string with ascii armour
//char output[] - string with the ascii armour off
//std::size_t capacity - size of input string
void armour_off(char *input, char output[], std::size_t capacity){
	//Loop through the array in groups of 5
	for (std::size_t k{0}; k < capacity / 5; ++k){
		//Read the values as base 85 and add to unsigned int
		unsigned int four_bytes {0};
		for (int x{0}; x < 5; x++){
			four_bytes *= 85;
			four_bytes += input[k * 5 + x] - '!';
		}
		//Assign values to output based on value of unsigned int modulo 256
		for (int x{3}; x >= 0; --x){
			output[k * 4 + x] = four_bytes % 256;
			four_bytes /= 256;
		}
	}//for (groups of 5 through input)
}//armour_off();

//================================================================================================

//NOTE: This function assumes that the array passed in contains NULL character.
std::size_t find_size(char *arr) {
//Return position of the first NULL character
	for (std::size_t x { 0 }; true; ++x) {
		if (arr[x] == '\0') {
			return x;
		}
	}
}//find_size();
