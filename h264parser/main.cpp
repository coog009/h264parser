#include <iostream>
#include <fstream>

#include "h264_bit_reader.h"
#include "h264_parser.h"

using namespace std;
using namespace media;

#define TEST_SIZE 20480

void usage()
{
	cout << "Usage:" << endl;
	cout << "  h264parser filename" << endl;
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		cout << "Invalid Parameter" << endl;
		usage();
	}

	ifstream in;
	in.open(argv[1], ios::in | ios::out | ios::binary);
	if (!in.is_open()) {
		cout << "file doesn't exist" << endl;
		return 0;
	}
	uint8 data[TEST_SIZE];
	uint8 *pdata = data;
	int left_size = TEST_SIZE;
	int nalu_size;

	in.read((char *)&nalu_size, 4);
	while (left_size > 0 && nalu_size < left_size) {
		in.read((char *)pdata, nalu_size);
		pdata += nalu_size;
		left_size -= nalu_size;

		if (!in.eof()) {
			in.read((char *)&nalu_size, 4);
		}
	}

	in.close();

	int data_size = pdata - data;

	H264Parser *p_parser = new H264Parser();
	p_parser->SetStream(data, data_size);

	H264NALU *nalu = new H264NALU();

	while (p_parser->AdvanceToNextNALU(nalu) != 11) {
		switch (nalu->nal_unit_type) {
		case H264NALU::Type::kSPS:
			int sps_id;
			p_parser->ParseSPS(&sps_id);
			cout << "get sps id:" << sps_id << endl;
			break;
		case H264NALU::Type::kPPS:
			int pps_id;
			p_parser->ParsePPS(&pps_id);
			cout << "get pps id:" << pps_id << endl;
			break;
		case H264NALU::Type::kSEIMessage:
			/* todo */
			break;
		case H264NALU::Type::kSliceDataA:
		case H264NALU::Type::kSliceDataB:
		case H264NALU::Type::kSliceDataC:
		case H264NALU::Type::kIDRSlice: 
		{
			H264SliceHeader *slice_header = new H264SliceHeader();
			p_parser->ParseSliceHeader(*nalu, slice_header);
			delete slice_header;
			break;
		}
		default:
			cout << "none" << endl;
			break;
		}
	}

	delete p_parser;
}