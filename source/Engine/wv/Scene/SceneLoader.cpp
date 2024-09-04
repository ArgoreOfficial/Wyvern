
#include <string>
#include <unordered_map>

#include <stdio.h>

struct node
{
	double prob, range_from, range_to;
};

double encoding( std::string s, std::unordered_map<char, node> arr )
{
	//cout << "\nEncoding\n";
	double low_v = 0.0, high_v = 1.0, diff = 1.0;
	//cout << "Symbol\tLow_v\tHigh_v\tdiff\n";
	for( int i = 0; i < s.size(); i++ )
	{
		high_v = low_v + diff * arr[ s[ i ] ].range_to;
		low_v = low_v + diff * arr[ s[ i ] ].range_from;
		diff = high_v - low_v;
		//cout << s[ i ] << "\t" << low_v << "\t" << high_v << "\t" << diff << endl;
	}
	return low_v;
}

std::string decoding( std::unordered_map<char, node> arr, double code_word, int len )
{
	char ch;
	std::string text = "";
	int j = 0;
	std::unordered_map<char, node>::iterator it;
	while( j < len )
	{
		for( it = arr.begin(); it != arr.end(); it++ )
		{
			char i = ( *it ).first;
			if( arr[ i ].range_from <= code_word && code_word < arr[ i ].range_to )
			{
				ch = i;
				code_word = ( code_word - arr[ i ].range_from ) / ( arr[ i ].range_to - arr[ i ].range_from );
				break;
			}
		}
		text += ch;
		j++;
	}
	return text;
}

// https://github.com/dmitrykravchenko2018/arithmetic_coding/blob/master/encode.cpp
void dothing()
{
	//this is an epic seytcee wsntshainntiewsp

	std::string encodeString = "this is an epic sen";
	std::unordered_map<char, node> arr;

	double rangeFrom = 0;
	for( size_t i = 0; i < encodeString.size(); i++ )
	{
		char letter = encodeString[ i ];

		auto search = arr.find( letter );
		if( search != arr.end() )
			continue;

		arr[ letter ] = {};
		arr[ letter ].prob = ( double )std::count( encodeString.begin(), encodeString.end(), letter ) / ( double )( encodeString.size() );
		arr[ letter ].range_from = rangeFrom;
		arr[ letter ].range_to = rangeFrom + arr[ letter ].prob;
		rangeFrom = arr[ letter ].range_to;
	}

	double codeword = encoding( encodeString, arr );

	printf( "\n%f\n", codeword );
	printf( "%s.\n", decoding( arr, codeword, encodeString.size() ).c_str() );
}
