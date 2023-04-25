#include <iconv.h> // 编码转换
#include <string>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

const char* FILE_PATH = "./assets/opencv.jpg";

using std::string;
using std::cout;
using std::endl;

std::string code_convert(const char *source_charset, const char *to_charset, const std::string& sourceStr);
std::string GbkToUtf8(const std::string& strGbk);
std::string Utf8ToGbk(const std::string& strUtf8);
std::string GbkToUnicode(const std::string& strGbk);
std::string UnicodeToGbk(const std::string& strGbk);



int main()
{
	string en_str{ "hello" };
	string zh_str{ "你好" };
	cout << en_str
		<< " en_str(hello) size=" << en_str.size()
		<< " strlen(en_str.data())=" << strlen(en_str.data()) << endl;
	cout << zh_str
		<< " zh_str(你好) size=" << zh_str.size()
		<< " strlen(en_str.data())=" << strlen(zh_str.data()) << endl;

	string u8_en_str = u8"hello";
	string u8_zh_str = u8"你好";
	cout << u8_en_str
		<< " add u8 prefix u8_en_str(hello) size=" << en_str.size()
		<< " strlen(u8_en_str.data())=" << strlen(u8_en_str.data()) << endl;
	cout << u8_zh_str
		<< " add u8 prefix u8_zh_str(你好) size=" << en_str.size()
		<< " strlen(u8_zh_str.data())=" << strlen(u8_zh_str.data()) << endl;
	
    //
	// std::string win_name(zh_str);
	// std::string win_name(u8_zh_str);

	/// 增加编码转换函数

	// zh_str
	std::string win_name = Utf8ToGbk(zh_str);
	// std::string win_name(zh_str); //  
	
	// u8_zh_str
	//std::string win_name = Utf8ToGbk(u8_zh_str);
	cout << "UTF8toGBK= " << win_name << endl;


	// 载入图像
	cv::Mat src_image = cv::imread(FILE_PATH);
	cv::namedWindow(win_name, cv::WINDOW_KEEPRATIO | cv::WND_PROP_ASPECT_RATIO);
	cv::imshow(win_name, src_image);

	// 等待任意键按下
	cv::waitKey(0);

	//getchar();
	return 0;
}


/**
 * @brief 
 * 
 * @param source_charset 源编码
 * @param to_charset 目标编码
 * @param sourceStr 源编码字符串
 * @return std::string 
 */
std::string code_convert(const char *source_charset, const char *to_charset, const std::string& sourceStr)
{
    // 获取转换句柄，void*类型
	iconv_t cd = iconv_open(to_charset, source_charset);
	if (cd == 0)
		return "";

	size_t inlen = sourceStr.size();
	char* inbuf = (char*)sourceStr.c_str();
	
	size_t MAX_LEN = 1024;
	// TODO: 支持的最大字节，这是个问题
	char *outbuf = new char[MAX_LEN];
	memset(outbuf, 0, MAX_LEN);

	char *poutbuf = outbuf;
    //多加这个转换是为了避免iconv这个函数出现char(*)[255]类型的实参与char**类型的形参不兼容
	if (iconv(cd, &inbuf, &inlen, &poutbuf, &MAX_LEN) == -1)
		return "";

	std::string strTemp(outbuf);
	iconv_close(cd);
	return strTemp;
}

/**
 * @brief gbk转UTF-8 
 * 
 * @param strGbk 传入的strGbk是GBK编码 
 * @return std::string 
 */
std::string GbkToUtf8(const std::string& strGbk)
{
	return code_convert("gb2312", "utf-8", strGbk);
}

/**
 * @brief UTF-8转gbk
 * 
 * @param strUtf8 
 * @return std::string 
 */
std::string Utf8ToGbk(const std::string& strUtf8)
{
	return code_convert("utf-8", "gb2312", strUtf8);
}

/**
 * @brief gbk转unicode,"UCS-2LE"代表unicode小端模式
 * 
 * @param strGbk 传入的strGbk是GBK编码 
 * @return std::string 
 */
std::string GbkToUnicode(const std::string& strGbk)
{
	return code_convert("gb2312", "UCS-2LE", strGbk);
}

/**
 * @brief unicode转gbk
 * 
 * @param strGbk 传入的strGbk是GBK编码 
 * @return std::string 
 */
std::string UnicodeToGbk(const std::string& strGbk)
{
	return code_convert("UCS-2LE", "gb2312", strGbk);
}

