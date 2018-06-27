#include <iostream>
#include "httplib.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/reader.h"

using  rapidjson::Document;
using  rapidjson::StringBuffer;
using  rapidjson::Writer;

#include "log4z.h"
using zsummer::log4z::ILog4zManager;

#include <opencv2/opencv.hpp>

static std::string base64Decode(const char* Data, int DataByte)
{
	//解码表
	const char DecodeTable[] =
	{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		62, // '+'
		0, 0, 0,
		63, // '/'
		52, 53, 54, 55, 56, 57, 58, 59, 60, 61, // '0'-'9'
		0, 0, 0, 0, 0, 0, 0,
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
		13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, // 'A'-'Z'
		0, 0, 0, 0, 0, 0,
		26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
		39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, // 'a'-'z'
	};
	//返回值
	std::string strDecode;
	int nValue;
	int i = 0;
	while (i < DataByte)
	{
		if (*Data != '\r' && *Data != '\n')
		{
			nValue = DecodeTable[*Data++] << 18;
			nValue += DecodeTable[*Data++] << 12;
			strDecode += (nValue & 0x00FF0000) >> 16;
			if (*Data != '=')
			{
				nValue += DecodeTable[*Data++] << 6;
				strDecode += (nValue & 0x0000FF00) >> 8;
				if (*Data != '=')
				{
					nValue += DecodeTable[*Data++];
					strDecode += nValue & 0x000000FF;
				}
			}
			i += 4;
		}
		else// 回车换行,跳过
		{
			Data++;
			i++;
		}
	}
	return strDecode;
}

static std::string base64Encode(const unsigned char* Data, int DataByte)
{
	//编码表
	const char EncodeTable[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	//返回值
	std::string strEncode;
	unsigned char Tmp[4] = { 0 };
	int LineLength = 0;
	for (int i = 0; i < (int)(DataByte / 3); i++)
	{
		Tmp[1] = *Data++;
		Tmp[2] = *Data++;
		Tmp[3] = *Data++;
		strEncode += EncodeTable[Tmp[1] >> 2];
		strEncode += EncodeTable[((Tmp[1] << 4) | (Tmp[2] >> 4)) & 0x3F];
		strEncode += EncodeTable[((Tmp[2] << 2) | (Tmp[3] >> 6)) & 0x3F];
		strEncode += EncodeTable[Tmp[3] & 0x3F];
		if (LineLength += 4, LineLength == 76) { strEncode += "\r\n"; LineLength = 0; }
	}
	//对剩余数据进行编码
	int Mod = DataByte % 3;
	if (Mod == 1)
	{
		Tmp[1] = *Data++;
		strEncode += EncodeTable[(Tmp[1] & 0xFC) >> 2];
		strEncode += EncodeTable[((Tmp[1] & 0x03) << 4)];
		strEncode += "==";
	}
	else if (Mod == 2)
	{
		Tmp[1] = *Data++;
		Tmp[2] = *Data++;
		strEncode += EncodeTable[(Tmp[1] & 0xFC) >> 2];
		strEncode += EncodeTable[((Tmp[1] & 0x03) << 4) | ((Tmp[2] & 0xF0) >> 4)];
		strEncode += EncodeTable[((Tmp[2] & 0x0F) << 2)];
		strEncode += "=";
	}

	return strEncode;
}

static std::string Mat2Base64(const cv::Mat &img, std::string imgType)
{
	//Mat转base64
	std::string img_data;
	std::vector<uchar> vecImg;
	std::vector<int> vecCompression_params;
	vecCompression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
	vecCompression_params.push_back(90);
	imgType = "." + imgType;
	cv::imencode(imgType, img, vecImg, vecCompression_params);
	img_data = base64Encode(vecImg.data(), vecImg.size());
	return img_data;
}

static cv::Mat Base2Mat(std::string &base64_data)
{
	cv::Mat img;
	std::string s_mat;
	s_mat = base64Decode(base64_data.data(), base64_data.size());
	std::vector<char> base64_img(s_mat.begin(), s_mat.end());
	img = cv::imdecode(base64_img, CV_LOAD_IMAGE_COLOR);
	return img;
}

StringBuffer getJson() {

	cv::Mat src = cv::imread("./1.png");
	cv::Size sz = cv::Size(src.cols / 2, src.rows / 2);
	cv::resize(src, src, sz);
	Base64 base;
	std::string dst=Mat2Base64(src, "jpg");
	LOGI("img data=" << dst);
	LOGI("img len=" << dst.size());
	StringBuffer s;
	Writer<StringBuffer> writer(s);
	writer.StartObject();
	writer.Key("time");
	writer.String("2018-06-07 12:12:56");
	writer.Key("location");
	writer.String("N302904.0000W1142418.0000");
	writer.Key("plateType");
	writer.String("blue");
	writer.Key("plateName");
	//writer.String(UnicodeToUtf8(L"鄂A77Y33"));
	writer.String("鄂A77Y33");
	writer.Key("confidence");
	writer.String("0.9999");
	writer.Key("imgType");
	writer.String("jpg");
	writer.Key("imgData");
	writer.String(dst.c_str());
	writer.EndObject();
	return s;
}

int main(int argc, char *argv[]) {
	ILog4zManager::getRef().start();
	LOGI("start");

	StringBuffer toPost = getJson();
	printf("to post=%s\n", toPost.GetString());

	httplib::Client client_post_local("192.168.0.179", 80);
	client_post_local.Post("/plateRecognize/reportData", toPost.GetString(), "application/json");
	LOGI("has post\n");

	getchar();
	return 0;
}
