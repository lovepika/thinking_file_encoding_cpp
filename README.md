> cpp source code file encoding thinking
> 
> repo link: https://github.com/lovepika/thinking_file_encoding_cpp

# cpp源码在跨平台场景下文件编码的思考

# 1.为什么写这篇文章

之前主要的开发环境主要是在Linux，文件编码选择了UTF-8. 
代码中也硬编码过中文(不是很好的方式哦)，平时的注释也是中文。
因为Linux系统下的缘故，没有出现过乱码之类的事情，之前没有认真思考过编码相关的问题。(其实是Linux下终端默认的UTF-8掩盖了问题。)

本篇文章将探讨下面几个问题:

- **cpp程序中都可以出现哪些字符，为什么我的程序字符乱码了**
- **cpp开发中源代码文件编码的一次梳理和思考**
- **来探讨一下在面对跨平台开发的时候该如何选择文件编码** `UTF-8 With BOM` ; `UTF-8` ; `GB2312/GBK`
- **Windows/linux终端中以及窗口标题如何正确显示中文**

# 2.cpp程序中可以出现哪些字符？

> ref link: https://learn.microsoft.com/en-us/cpp/cpp/character-sets?view=msvc-150

**Basic source character set**

The C++ standard specifies a basic source character set that may be used in source files. To represent characters outside of this set, additional characters can be specified by using a universal character name. The MSVC implementation allows additional characters. The basic source character set consists of 96 characters that may be used in source files. This set includes the space character, horizontal tab, vertical tab, form feed and new-line control characters, and this set of graphical characters:

```
a b c d e f g h i j k l m n o p q r s t u v w x y z

A B C D E F G H I J K L M N O P Q R S T U V W X Y Z

0 1 2 3 4 5 6 7 8 9

_ { } [ ] # ( ) < > % : ; . ? * + - / ^ & | ~ ! = , \ " '
```

*翻译:*

C++ 标准指定可在源文档中使用的基本源字符集。若要表示此集合之外的字符，可以使用通用字符名称指定其他字符。MSVC 实现允许使用其他字符。基本源字符集由 96 个字符组成，可用于源代码文档。这组包括空格字符、水平制表符、垂直制表符、换页符和换行符控制字符，以及这组图形字符:


**Execution character sets**

The execution character sets represent the characters and strings that can appear in a compiled program. These character sets consist of all the characters permitted in a source file, and also the control characters that represent alert, backspace, carriage return, and the null character. The execution character set has a locale-specific representation.

*翻译:*

执行字符集表示可以出现在已编译程序中的字符和字符串。 这些字符集由源文档中允许的所有字符组成，以及表示警告、退格、回车和空字符的控制字符。执行字符集具有特定于**语言环境**的表示。 

**从这里我们得知：与源代码文档中的字符和语言环境的有关**。


# 3.windows下MSVC编译器提供了如下的编译选项

- [`/utf-8`](https://learn.microsoft.com/en-us/cpp/build/reference/utf-8-set-source-and-executable-character-sets-to-utf-8?view=msvc-170) Specifies both the source character set and the execution character set as UTF-8.
- [`/source-charset`](https://learn.microsoft.com/en-us/cpp/build/reference/source-charset-set-source-character-set?view=msvc-150) This option lets you specify the source character set **for your executable**.
- [`/execution-charset`](https://learn.microsoft.com/en-us/cpp/build/reference/execution-charset-set-execution-character-set?view=msvc-150) This option lets you specify the execution character set **for your executable**.

**注意加粗黑体, for your executable**

## `/utf-8`

You can use the `/utf-8` option to specify both the source and execution character sets as encoded by using UTF-8. 
It's **equivalent** to specifying `/source-charset:utf-8 /execution-charset:utf-8` on the command line. 

Any of these options also enables the `/validate-charset` option by default. For a list of supported code page identifiers and character set names, see [code-page-identifiers](https://learn.microsoft.com/en-us/windows/win32/Intl/code-page-identifiers).

By default, Visual Studio detects a byte-order mark to determine if the source file is in an encoded Unicode format, for example, UTF-16 or UTF-8. **If no byte-order mark is found, it assumes that the source file is encoded in the current user code page**, unless you've specified a code page by using `/utf-8` or the `/source-charset` option. Visual Studio allows you to save your C++ source code in any of several character encodings. For information about source and execution character sets, see [Character sets](https://learn.microsoft.com/en-us/cpp/cpp/character-sets?view=msvc-150) in the language documentation.

*翻译*：

`/utf-8` 选项 等价于 `/source-charset:utf-8` + `execution-charset:utf-8`。

我们可以指定任一有效的 `/validate-charset`选项, 支持的有：[code-page-identifiers](https://learn.microsoft.com/en-us/windows/win32/Intl/code-page-identifiers)，

默认情况下,VS会检测字节顺序标记, 以确定源文件是否采用编码的 Unicode 格式，例如 UTF-16 或 UTF-8。

如果没有找到字节顺序标记，则假定源文件是在当前用户代码页中编码的(中文环境下的电脑就会把源代码文件当作gb2312编码的文件来处理)，除非使用了选项 `/utf-8` 或者`/source-charset` 指定代码页。
VS允许把C++源代码保存在几种字符编码中的任何一种中。(用`高级保存选项`设置)
有关源和执行字符集的信息, 看[Character sets](https://learn.microsoft.com/en-us/cpp/cpp/character-sets?view=msvc-150)


## `/source-charset`

在源文件包含**基本源字符集**中`未表示的字符`时，可以使用 `/source-charset` 选项指定要使用的**扩展源字符集**。

**源字符集是用于解释程序源文本的编码。 它转换为编译前用作预处理阶段输入的内部表示形式**。 

**然后，内部表示形式将转换为执行字符集，以在可执行文件中存储字符串和字符值**。

可以使用 `IANA` 或 `ISO` 字符集名称，也可以使用点 `(.)`，后跟指定要使用的字符集代码页标识符的 `3-5` 小数位数。 有关受支持的代码页标识符和字符集名称的列表，请参阅[代码页标识符](https://learn.microsoft.com/zh-cn/windows/win32/Intl/code-page-identifiers)。

默认情况下，Visual Studio 会检测字节顺序标记，以确定源文件是否采用编码的 Unicode 格式，例如 UTF-16 或 UTF-8。 如果未找到字节顺序标记，则假定源文件在当前用户代码页中编码，除非使用 /source-charset 或 /utf-8 选项指定字符集名称或代码页。 Visual Studio 允许将 C++ 源代码保存在任意几个字符编码中。 有关源字符集和执行字符集的详细信息，请参阅语言文档中的字符集。


## `/execution-charset`

可以使用 `/execution-charset` 选项来指定执行字符集。 **执行字符集是一种编码，用于在所有预处理步骤之后输入到编译阶段的程序文本**。

**此字符集用于编译代码中任何字符串或字符文字的内部表示**。 

**设置此选项以指定在源文件包含在基本执行字符集中无法表示的字符时使用的扩展执行字符集。**

可以使用 `IANA 或 ISO` 字符集名称，也可以使用点 (`.`)，后跟指定要使用的字符集代码页标识符的 `3-5` 小数位数。 有关受支持的代码页标识符和字符集名称的列表，请参阅[代码页标识符](https://learn.microsoft.com/zh-cn/windows/win32/Intl/code-page-identifiers)。


# 4.Window下文件编码和MSVC编译选项的思考

从上面的参考我们可以得知，VS通过`byte-order mark(BOM)`来探测源代码文件编码是否为Unicode编码, 探测不到就用你**语言区域**的`code-page-identifiers`来作为 `/source-charset` 和 `execution-charset`的值。(**也就是缺省则是按照系统当前locale的编码**，我当前的win10是中文简体)

windows10中文简体系统语言区域下是 `936代码页 	gb2312 	ANSI/OEM Simplified Chinese (PRC, Singapore); Chinese Simplified (GB2312)` 

后面我在测试过程中发现，只要没有指定选项MSVC编译器选项`/source-charset` 和 `execution-charset`。有自己的默认行为(暂时没看到值在哪里设置上去的).(测试VS版本为：VS2017 15.9.54)

中文环境下的默认行为相等于默认增加了下面两个选项 `/source-charset:gb2312` 和 `execution-charset:gb2312`。

假设我的源代码编码为`UTF-8`. VS因为探测不到`BOM`，会把源代码文件当作`.936代码页的文件编码，也就是gb2312`来处理，编译选项默认行为类似 `/source-charset:gb2312` 和 `execution-charset:gb2312`。

假设我的源代码编码为`UTF-8 With BOM`. VS因为探测到`BOM`，则把源代码当作`UTF-8 With BOM`处理。注意因为源代码是UTF-8 With BOM，编译器行为并没有表现为 `/source-charset:utf-8`。而是编译选项默认行为类似 `execution-charset:gb2312`。 
TODO:这也是比较疑惑的地方，也可能是VS下`UTF-8 With BOM` 和 `UTF-8`代码页都是65001相同的缘故吗？探测到源代码是Unicode编码就只默认设置`execution-charset:gb2312`


# 5.在实践中去验证问题

## UTF-8 No BOM

文件编码：UTF-8 No BOM

测试程序

```cpp
#include <string>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

const char* FILE_PATH = "d:\\capture_main.jpg";

using std::string;
using std::cout;
using std::endl;

int main() 
{
	string en_str{"hello"};
	string zh_str{"你好"};
	cout << en_str 
		<< " en_str(hello) size=" << en_str.size() 
		<< " strlen(en_str.data())=" << strlen(en_str.data()) << endl;
	cout << zh_str 
		<<" zh_str(你好) size=" << zh_str.size() 
		<< " strlen(en_str.data())=" <<  strlen(zh_str.data()) << endl;
	
	// 需要说明的是 文件编码本身就是UTF-8时候，u8前缀可有可无，效果一样
	// 可以查看内存中zh_str 和 u8_zh_str 这里还是写上u8前缀
	string u8_en_str = u8"hello";
	string u8_zh_str = u8"你好";
	cout << u8_en_str 
		<< " add u8 prefix u8_en_str(hello) size=" << en_str.size() 
		<< " strlen(u8_en_str.data())=" << strlen(u8_en_str.data()) << endl;
	cout << u8_zh_str 
		<<" add u8 prefix u8_zh_str(你好) size=" << en_str.size() 
		<< " strlen(u8_zh_str.data())=" << strlen(u8_zh_str.data()) << endl;
  
  std::string win_name(zh_str);
  // std::string win_name(u8_zh_str);

	// 载入图像
  cv::Mat src_image = cv::imread(FILE_PATH);
	// window
	cv::namedWindow(win_name, cv::WINDOW_KEEPRATIO | cv::WND_PROP_ASPECT_RATIO);
  cv::imshow(win_name, src_image);
  
  // 等待任意键按下
  cv::waitKey(0);

	//getchar();

	return 0;
}

```

### 无手动增加MSVC编译选项

实验1： `std::string win_name(zh_str);` 作为窗口名字
结果：窗口文字乱码

实验2： `std::string win_name(u8_zh_str);` 作为窗口名字
结果：窗口文字乱 (乱码文字上面不一样)

> 注: 源代码是UTF-8 No BOM，却没有增加编译选项，VS探测不到文件编码时将当作当前代码页处理，这里为`936`. 没有指定编译选项相当于编译选项为： `/source-charset:gbk /execution-charset:gbk`
>
> 想要正确解析源代码文件就需要增加 `/source-charset:utf-8`


### 编译选项 `/source-charset:utf-8`

实验1： `std::string win_name(zh_str);` 作为窗口名字
结果：窗口文字正常

实验2： `std::string win_name(u8_zh_str);` 作为窗口名字
结果：窗口文字乱码


### 增加MSVC编译选项 `/source-charset:utf-8 /execution-charset:gbk`

> 选项里面的gbk换成gb2312也行

实验1： `std::string win_name(zh_str);` 作为窗口名字
结果：窗口文字正常

实验2： `std::string win_name(u8_zh_str);` 作为窗口名字
结果：窗口文字乱码

> 问题发现:从这上面这是个6个实验来看，我们验证了 `/source-charset /execution-charset`缺省状态会使用当前系统语言区域代码页作为值

### 增加MSVC编译选项`/utf-8`

> `/utf-8` 相当于 `/source-charset:utf-8 /execution-charset:utf-8`

实验1： `std::string win_name(zh_str);` 作为窗口名字
结果：窗口文字乱码

实验2： `std::string win_name(u8_zh_str);` 作为窗口名字
结果：窗口文字乱 (**乱码文字和上面一样,都是`浣犲ソ`**)


## UTF-8 No BOM 和 增加编码转换函数

```cpp
#include <iconv.h>
#include <string>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

const char* FILE_PATH = "d:\\capture_main.jpg";

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
	string en_str{"hello"};
	string zh_str{"你好"};
	cout << en_str 
		<< " en_str(hello) size=" << en_str.size() 
		<< " strlen(en_str.data())=" << strlen(en_str.data()) << endl;
	cout << zh_str 
		<<" zh_str(你好) size=" << zh_str.size() 
		<< " strlen(en_str.data())=" <<  strlen(zh_str.data()) << endl;
	
	// 需要说明的是 文件编码本身就是UTF-8时候，u8前缀可有可无，效果一样
	// 可以查看内存中zh_str 和 u8_zh_str 这里还是写上u8前缀
	string u8_en_str = u8"hello";
	string u8_zh_str = u8"你好";
	cout << u8_en_str 
		<< " add u8 prefix u8_en_str(hello) size=" << en_str.size() 
		<< " strlen(u8_en_str.data())=" << strlen(u8_en_str.data()) << endl;
	cout << u8_zh_str 
		<<" add u8 prefix u8_zh_str(你好) size=" << en_str.size() 
		<< " strlen(u8_zh_str.data())=" << strlen(u8_zh_str.data()) << endl;
	
	 // std::string win_name(zh_str);
   // std::string win_name(u8_zh_str);
  
  std::string win_name = Utf8ToGbk(zh_str);
  // std::string win_name = Utf8ToGbk(u8_zh_str);
  
  cout <<"UTF8toGBK= " << win_name << endl;
	//
	// 载入图像
    cv::Mat src_image = cv::imread(FILE_PATH);
	  cv::namedWindow(win_name, cv::WINDOW_KEEPRATIO | cv::WND_PROP_ASPECT_RATIO);
    cv::imshow(win_name, src_image);
    // 等待任意键按下
    cv::waitKey(0);

	//getchar();
	return 0;
}


//编码转换，source_charset是源编码，to_charset是目标编码
std::string code_convert(const char *source_charset, const char *to_charset, const std::string& sourceStr) //sourceStr是源编码字符串
{
	iconv_t cd = iconv_open(to_charset, source_charset);//获取转换句柄，void*类型
	if (cd == 0)
		return "";
 
    size_t inlen = sourceStr.size();
	char* inbuf = (char*)sourceStr.c_str();
	size_t MAX_LEN = 1024; // 最大字节
	char *outbuf = new char[MAX_LEN];
	memset(outbuf, 0, MAX_LEN);
 
	char *poutbuf = outbuf; //多加这个转换是为了避免iconv这个函数出现char(*)[255]类型的实参与char**类型的形参不兼容
	if (iconv(cd, &inbuf, &inlen, &poutbuf,&MAX_LEN) == -1)
		return "";
 
    std::string strTemp(outbuf);//此时的strTemp为转换编码之后的字符串
	iconv_close(cd);
	return strTemp;
}

//gbk转UTF-8  
std::string GbkToUtf8(const std::string& strGbk)// 传入的strGbk是GBK编码 
{
	return code_convert("gb2312", "utf-8",strGbk);
}
 
//UTF-8转gbk
std::string Utf8ToGbk(const std::string& strUtf8)
{
	return code_convert("utf-8", "gb2312", strUtf8);
}
 
//gbk转unicode,"UCS-2LE"代表unicode小端模式
std::string GbkToUnicode(const std::string& strGbk)// 传入的strGbk是GBK编码 
{
	return code_convert("gb2312", "UCS-2LE",strGbk);
}
 
//unicode转gbk
std::string UnicodeToGbk(const std::string& strGbk)// 传入的strGbk是GBK编码 
{
	return code_convert("UCS-2LE", "gb2312",strGbk);
}
```

### 编译选项：`/utf-8`

实验1 
窗口名：`std::string win_name = Utf8ToGbk(zh_str);`
结果： 窗口文字中文不乱码 正常

实验2
窗口名：`std::string win_name = Utf8ToGbk(u8_zh_str);`
结果： 窗口文字中文不乱码 正常


### 编译选项：`/source-charset:utf-8`

> 注: 中文环境 `/source-charset:utf-8` 等价于 `/source-charset:utf-8 /execution-charset:gbk`

**实验1:** 
窗口名：`std::string win_name = Utf8ToGbk(zh_str);`
结果： 程序crash 

原因在这句转码处理结果为空 `std::string win_name = Utf8ToGbk(zh_str);` VS查看内存信息，`zh_str`在内存种已经是`你好`，说明这里被gbk编码的。 **加不加u8的区别出来了！(Windows让问题暴漏出来了，Linux下编码统一UTF-8隐藏了这些潜在的问题)**。  可以把编码转换去掉来验证
```cpp
// std::string win_name = Utf8ToGbk(zh_str);
std::string win_name(zh_str); // 去掉编码直接作为窗口名字的参数，不闪退！
//...
```

```cpp
#include <iconv.h>
#include <string>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

const char* FILE_PATH = "d:\\capture_main.jpg"; // 2560*1920 测试大图像下的那么Window的参数

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
	// std::string win_name = Utf8ToGbk(zh_str);
	std::string win_name(zh_str); // 测试猜想
	
	// u8_zh_str
	std::string win_name = Utf8ToGbk(u8_zh_str);
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


//编码转换，source_charset是源编码，to_charset是目标编码
std::string code_convert(const char *source_charset, const char *to_charset, const std::string& sourceStr) //sourceStr是源编码字符串
{
	iconv_t cd = iconv_open(to_charset, source_charset);//获取转换句柄，void*类型
	if (cd == 0)
		return "";

	size_t inlen = sourceStr.size();
	char* inbuf = (char*)sourceStr.c_str();
	//char outbuf[255];//这里实在不知道需要多少个字节，这是个问题
	size_t MAX_LEN = 1024;
	char *outbuf = new char[MAX_LEN];
	memset(outbuf, 0, MAX_LEN);

	char *poutbuf = outbuf; //多加这个转换是为了避免iconv这个函数出现char(*)[255]类型的实参与char**类型的形参不兼容
	if (iconv(cd, &inbuf, &inlen, &poutbuf, &MAX_LEN) == -1)
		return "";

	std::string strTemp(outbuf);//此时的strTemp为转换编码之后的字符串
	iconv_close(cd);
	return strTemp;
}

//gbk转UTF-8  
std::string GbkToUtf8(const std::string& strGbk)// 传入的strGbk是GBK编码 
{
	return code_convert("gb2312", "utf-8", strGbk);
}

//UTF-8转gbk
std::string Utf8ToGbk(const std::string& strUtf8)
{
	return code_convert("utf-8", "gb2312", strUtf8);
}

//gbk转unicode,"UCS-2LE"代表unicode小端模式
std::string GbkToUnicode(const std::string& strGbk)// 传入的strGbk是GBK编码 
{
	return code_convert("gb2312", "UCS-2LE", strGbk);
}

//unicode转gbk
std::string UnicodeToGbk(const std::string& strGbk)// 传入的strGbk是GBK编码 
{
	return code_convert("UCS-2LE", "gb2312", strGbk);
}
```


**实验2**
窗口名：`std::string win_name = Utf8ToGbk(u8_zh_str);`
结果： 窗口文字中文不乱码 正常


## UTF-8 With BOM

### 编译选项： 无
(UTF-8 With BOM编码的情况下，没有增加选项则等价 `/execution-charset:gbk`)

实验1： `std::string win_name(zh_str);` 作为窗口名字
结果：窗口文字正常不乱码

实验2： `std::string win_name(u8_zh_str);` 作为窗口名字
结果：窗口文字乱码


### 编译选项： `/execution-charset:utf-8`

实验1： `std::string win_name(zh_str);` 作为窗口名字
结果：窗口文字乱码

实验2： `std::string win_name(u8_zh_str);` 作为窗口名字
结果：窗口文字乱码


## UTF-8 With BOM 和 增加编码转换函数

### 编译选项： 无

(UTF-8 With BOM编码的情况下，没有增加选项则等价 `/execution-charset:gbk`)

**实验1**:
窗口名：`std::string win_name = Utf8ToGbk(zh_str);`
结果： 程序crash

原因和上面的 **文件编码utf-8 no bom `/source-charset:utf-8 /execution-charset:gbk`组合的情况一样**！ 
**修复方式一样**：
```cpp
// std::string win_name = Utf8ToGbk(zh_str);
std::string win_name(zh_str); // 去掉编码直接作为窗口名字的参数，不闪退！
//...
```

**猜想： 文件编码使用`UTF-8 With BOM`, 但是没有增加选项选项，则MSVC如果没有指定编译选项的情况下，则会使用`文件编码 + 语言环境的Execution character sets`**


```cpp
#include <iconv.h>
#include <string>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

const char* FILE_PATH = "d:\\capture_main.jpg"; // 2560*1920 测试大图像下的那么Window的参数

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
	// std::string win_name = Utf8ToGbk(zh_str);
	std::string win_name(zh_str); // 验证猜想
	
	// u8_zh_str
	std::string win_name = Utf8ToGbk(u8_zh_str);
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


//编码转换，source_charset是源编码，to_charset是目标编码
std::string code_convert(const char *source_charset, const char *to_charset, const std::string& sourceStr) //sourceStr是源编码字符串
{
	iconv_t cd = iconv_open(to_charset, source_charset);//获取转换句柄，void*类型
	if (cd == 0)
		return "";

	size_t inlen = sourceStr.size();
	char* inbuf = (char*)sourceStr.c_str();
	//char outbuf[255];//这里实在不知道需要多少个字节，这是个问题
	size_t MAX_LEN = 1024;
	char *outbuf = new char[MAX_LEN];
	memset(outbuf, 0, MAX_LEN);

	char *poutbuf = outbuf; //多加这个转换是为了避免iconv这个函数出现char(*)[255]类型的实参与char**类型的形参不兼容
	if (iconv(cd, &inbuf, &inlen, &poutbuf, &MAX_LEN) == -1)
		return "";

	std::string strTemp(outbuf);//此时的strTemp为转换编码之后的字符串
	iconv_close(cd);
	return strTemp;
}

//gbk转UTF-8  
std::string GbkToUtf8(const std::string& strGbk)// 传入的strGbk是GBK编码 
{
	return code_convert("gb2312", "utf-8", strGbk);
}

//UTF-8转gbk
std::string Utf8ToGbk(const std::string& strUtf8)
{
	return code_convert("utf-8", "gb2312", strUtf8);
}

//gbk转unicode,"UCS-2LE"代表unicode小端模式
std::string GbkToUnicode(const std::string& strGbk)// 传入的strGbk是GBK编码 
{
	return code_convert("gb2312", "UCS-2LE", strGbk);
}

//unicode转gbk
std::string UnicodeToGbk(const std::string& strGbk)// 传入的strGbk是GBK编码 
{
	return code_convert("UCS-2LE", "gb2312", strGbk);
}
```

程序正常运行，窗口中文也显示正常！ 

另外可以在编译选项中添加 `/execution-charset:gbk` 验证我们的猜想。

**我们得出一个总要的结论**：

`文件编码为UTF-8 With BOM` + `添加编译选项 /execution-charset:gbk` 无需增加编码转换函数可以实现Windows下窗口中文的正常显示。

```shell
hello en_str(hello) size=5 strlen(en_str.data())=5
你好 zh_str(你好) size=4 strlen(en_str.data())=4
hello add u8 prefix u8_en_str(hello) size=5 strlen(u8_en_str.data())=5
浣犲ソ add u8 prefix u8_zh_str(你好) size=5 strlen(u8_zh_str.data())=6
```
`浣犲ソ` 是因为在内存数据是UTF-8 With BOM编码的字符串的直接在windows的GBK终端中打印肯定是不行的。
注意看 ` zh_str(你好) u8_zh_str(你好)` 这两个硬编码的字符串,都是UTF-8 With BOM硬编码的字符串，在windows的GBK终端中打印正常。

这就是 `文件编码为UTF-8 With BOM` + `添加编译选项 /execution-charset:gbk` 的带来的能力。

**实验2**
窗口名：`std::string win_name = Utf8ToGbk(u8_zh_str);`
结果： 转换成功，窗口中文正常


### 编译选项 `/execution-charset:utf-8`

**实验1**
窗口名：`std::string win_name = Utf8ToGbk(zh_str);`
结果： 窗口中文正常 不乱码

**实验2**
窗口名： `std::string win_name = Utf8ToGbk(u8_zh_str);`
结果： 窗口中文正常 不乱码


# 6.跨win/linux项目的编码选择和编译选项设置该如何选?

首选编码选择： `UTF-8 With BOM`

**可选**：如果需要**不增加字符串编码转换函数的情况下**在cmd窗口标题显示中文，或者要打印硬编码的中文，还需要在VS下设置MSVC编译选项：`/execution-charset:gbk` 

> 为什么还需要`/execution-charset:gbk`?
> 
> Windows下文件编码虽然使用了utf-8 with bom，但是由于没有增加编译选项`/execution-charset` 选项，源代码文件编码能识别为UTF-8(`/source-charset:`这部分不用关心), 但是由于没有指定`/execution-charset`，MSVC的处理是**如果没有指定则会应用当前系统代码页**, 源代码中没有显示使用`u8`前缀字符串字面量仍然会使用系统当前代码页编码字符串(Windows下这种操作，我认为很坑的事情)。
> 
> 所以如果你需要明确表示一个字符串字面量是UTF-8编码的，最好的方法就是使用`u8`前缀。


**基于CMake项目增加如下配置**

```cmake
if(MSVC)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /execution-charset:gbk")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /execution-charset:gbk")
endif()
```

**如果不涉及到中文**，其实，编码方案方案继续用`UTF-8`没问题，就是需要增加MSVC编译选项 `/utf-8` (VS2015起)。

**涉及中文**

另外方案就是：文件编码用`UTF-8`，增加MSVC编译选项 `/source-charset:utf-8 /execution-charset:gbk`。 但是UTF-8编码如果在Windows下不带BOM，编译项目经常看到大量警告。

如果非要用`UTF-8`, 并且不想引入GBK的字符集，则增加 `/utf-8` 选项， 并且涉及到窗口标题，终端文字的中文显示，windows下需要特殊处理，进行一次转码`Utf8ToGbk`。 Linux下不需要，默认就是UTF-8. 
**也就是说，源代码使用UTF-8，那么设置了`/utf-8` 选项，就要转码函数!, 其实这样更清晰，无非就是不同平台用转码特殊处理下。**


