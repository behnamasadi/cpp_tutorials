

```

 <ios>        <istream>       <iostream>        <fstream>         <sstream>

                 |------------------------------->ifstream
ios_base      istream
   |         ↗   |----------------------------------------------->istringstream
   |        /    |
   |       /     |
   ↓      /      ↓------------------------------->fstream
   ios        iostream
          \      ↑----------------------------------------------->stringstream
           \     |
            \    |------------------------------->ofstream
             ↘ ostream
                 |------------------------------------------------>ostrngstream

             <streambuf>
                  |------------------------------->filebuf
              streambuf
                  |------------------------------------------------->stringbuf

```

Refs: [1](http://www.cplusplus.com/reference/iolibrary/)

Stream:
A stream is an abstraction that represents a device (keyboard, files, network, ...) on which input and ouput operations are performed. 
A stream can basically be represented as a source or destination of characters of indefinite length.

Streambuf:
I/O is an expensive operation, so to reduce the number of I/O operations the system store the information in a temporary 
memory location, and delay the I/O operation to a moment when it has a good amount of data. This way you've a much smaller 
number of I/O operations, what means, a faster application.

A stream buffer is an object in charge of performing the reading and writing operations of the stream object.
It is responsible for communicating with external devices. When you read or write from cin or cout, you are actually 
reading/ writing from cin/cout streambuff which is controlled by operating system. Operating system puts the data there and
your application can read or write from there.
ref: https://stackoverflow.com/questions/29176636/can-someone-please-explain-how-stdio-buffering-works

Predefined Stream Objects
1) cin
typedef std::istream	std::basic_istream<char>;
extern std::istream cin;

2) cerr  

typedef std::ostream	std::basic_ostream<char>;
extern std::ostream cerr;

3) clog
extern ostream clog;

4) cout
extern ostream cout;





Formatted vs Unformatted I/O Function

Formatted: These functions allow to supply input or display output in user desired format, i.e. printf() and scanf()
Unformatted: they do not allow to supply input or display output in user desired format, i.e. getch(), getche(), getchar(), gets(), puts(),
putchar()



//////////////////////////////////////  Manipulators //////////////////////////////////////

Manipulators are helping functions that can modify the input/output stream.
It does not mean that we change the value of a variable, it only modifies the I/O stream using
insertion (<<) and extraction (>>) operators.

Types of Manipulators:
1) Manipulators without arguments: The most important manipulators defined by the IOStream library are provided below.
    I) endl: It is defined in ostream. It is used to enter a new line and after entering a new line it flushes
        (i.e. it forces all the output written on the screen or in the file) the output stream.

    II) ws: It is defined in istream and is used to ignore the whitespaces in the string sequence.
    III) ends: It is also defined in ostream and it inserts a null character into the output stream. It typically works with
        std::ostrstream, when the associated output buffer needs to be null-terminated to be processed as a C string.
    IV) flush: It is also defined in ostream and it flushes the output stream i.e. it forces all the output written
        on the screen or in the file. Without flush, the output would be the same but may not appear in real-time.

2) Manipulators with Arguments:

    I) Some important manipulators in <iomanip> are:
        a)setw (val): It is used to sets the field width in output operations.
        b)setfill (c): It is used to fill the character ‘c’ on output stream.
        c)setprecision (val): It sets val as the new value for the precision of floating-point values.
        d)setbase(val): It is used to set the numeric base value for numeric values.
        e)setiosflags(flag): It is used to sets the format flags specified by parameter mask.
        f)resetiosflags(m): It is used to resets the format flags specified by parameter mask.
    II)Some important manipulators in <ios> are:
        a)showpos: It forces to show a positive sign on positive numbers.
        b)noshowpos: It forces not to write a positive sign on positive numbers.
        c)showbase: It indicates numeric base of numeric values.
        d)uppercase: It forces uppercase letters for numeric values.
        e)nouppercase: It forces lowercase letters for numeric values.
        f)fixed: It uses decimal notation for ?oating-point values.
        g)scientific: It use scientific floating-point notation.
        h)hex: Read and write hexadecimal values for integers and it works same as the setbase(16).
        i)dec: Read and write decimal values for integers i.e. setbase(10).
        j)oct: Read and write octal values for integers i.e. setbase(10).
        k)left: It adjust output to the left.
        l)right: It adjust output to the right.

*/

////////////////////////////////////// Reading, Writting Files //////////////////////////////////////

void readingWrittingFilesExample()
{
/*
Available Modes for Opening a File:

Flag          Application          Meaning
___________________________________________________________________________
ios::app         out               Always append output to the end of the file
___________________________________________________________________________
ios::ate         out               Open and seek to end-of-file ("at end")
___________________________________________________________________________
ios::binary      in, out           Open file in binary mode (as opposed to
text mode)
___________________________________________________________________________
ios::in          in                Open a file for input
___________________________________________________________________________
ios::nocreate    in, out           If file doesn't exist, don't create it
___________________________________________________________________________
ios::noreplace   out               Don't delete the file (open fails if file exists
                                   unless you specify appor ate)
___________________________________________________________________________
ios::out         out               Open file for output
___________________________________________________________________________
ios::trunc       out               Truncate file to zero length if it already exists 
                                   (default if file exists and app or ate is not specified)
___________________________________________________________________________

modes are bitset. For instance, ios::app might equal 00000001, ios::ate might equal 00000010,
ios::out might equal 00000100, etc. So each mode corresponds to one bit which can be 0 or 1.
This means that more than one mode's value can be set at the same time using the arithmetic OR.

*/
    {
        std::ifstream inputfile("inputfile.txt");
        std::ofstream outputfile("outputfile.txt");
        float f;
        while(inputfile>>f)//detects end-of-file and exits loop
        {
            outputfile << "f = " << f << std::endl;
        }
        inputfile.close();
        outputfile.close();
    }
    //simple writting
    {
        std::ofstream myfile;
        myfile.open ("ReadingWritingIntoFile.txt",std::ofstream::ate | std::ofstream::app);
        myfile << "Writing this to a file.\n";
        myfile<<std::bitset<8>(14); // it will write 00001110
        myfile<<std::complex<double>(2,3);
        myfile.close();
    }

    //advance writing
    {
        std::ofstream myfile("ReadingWritingIntoFile.txt",std::ofstream::in | std::ofstream::out);
        myfile.seekp( 10,std::ios::beg);// Move the output pointer 10 char from begining
        myfile<<"something at the begining";

        myfile.seekp( -5,std::ios::end);// Move the output pointer 5 char before end
        myfile<<"something at the end";
    }

    //reading chars
    {
        std::ifstream myfile("ReadingWritingIntoFile.txt");
        int i;
        myfile>>i;
        if(myfile.good())
        {
            std::cout<<"Reading successd "<<std::endl;
        }else
        {
            std::cout<<"Reading failed "<<std::endl;
        }
    }

    //reading file at once
    {
        std::string path_to_file;
        std::ifstream in(path_to_file.c_str());
        std::stringstream buffer;
        buffer << in.rdbuf();
        std::string contents(buffer.str());
        std::cout<<contents <<std::endl;
    }

    //reading line by line
    {
        std::string line;
        std::ifstream myfile ("src/ReadWriteFile/example.txt");
        if (myfile.is_open())
        {
            while ( getline (myfile,line) )
            {
            //getline (myfile,line);
            std::cout <<"..." <<line << "..."<<std::endl;
            }
            myfile.close();
        }
        else
        {
            std::cout << "Unable to open file...";
        }
    }
}

////////////////////////////////////// stringstream //////////////////////////////////////

void stringstreamExample()
{
/*
typedef basic_stringstream<char> stringstream;

ios_base      istream
   |         ↗   |
   |        /    |
   |       /     |
   ↓      /      ↓
   ios        iostream
          \      ↑    ------------------------------------------->stringstream
           \     |
            \    |
             ↘ ostream


Objects of this class use a "string buffer" that contains a sequence of characters. This sequence of characters
can be accessed directly as a string object, using member str().
You can read from the string as if it were a stream (like cin).


Important methods are:
clear() — to clear the stream
str() — to get and set string object whose content is present in stream.
insertion (<<) operator  — add a string to the stringstream object. This operator has been overloaded with
various data types, so you can do  stringstream<<int or stringstream<<double etc.

extraction (>>) operator — read something from the stringstream object until it encouter a white space.

*/
    std::stringstream ss;

    std::string name="behnam";
    int age=33;
    ss<<"name: "<<name;
    ss<<" age: "<<age;

    std::cout<< "Accessing data stored in stringstream via .str() call:"<<std::endl;

    std::cout<< ss.str() <<std::endl;

    std::string word;

    std::cout<< "Accessing data stored in stringstream via extraction (>>) operator:"<<std::endl;

    ss<<"23 4 5.0";

    while(ss>>word)
    {
        std::cout<<word <<std::endl;
    }

    std::cout<<"//////////////////////// Counting Words Frquency ////////////////////////////"<<std::endl;


    std::stringstream wordsFrquencyStream("a b bb c a dd d");


    std::map<std::string,int> wordsFrquency;
    while(wordsFrquencyStream>>word)
    {
        wordsFrquency[word]++;
    }
    std::cout<<"Frequecy of words in "<<wordsFrquencyStream.str() <<std::endl;

    for(auto i:wordsFrquency)
        std::cout<<i.first<<":" <<i.second <<std::endl;


    std::cout<<"///////////////////////////// Hex Decimal /////////////////////////////////"<<std::endl;


    std::stringstream hexDecimalStream;
    hexDecimalStream<<std::hex<<12;
    std::cout<<"0x" << hexDecimalStream.str() <<std::endl;

    unsigned int x;
    hexDecimalStream>>x;
    std::cout<< x<<std::endl;


    std::cout<<"the hexadecimal value of 12 is:"<<std::endl;
    std::cout<<"0x"<<std::setbase(16)<<12<<std::endl;
}

////////////////////////////////////// streambuf //////////////////////////////////////



void streambuf()
{
/*
	Streams Objects in C++ are mainly of three types :

	istream : Stream object of this type can only perform input operations from the stream
	ostream : These objects can only be used for output operations.
	iostream : Can be used for both input and output operations

	All stream objects also have an associated data member of class streambuf. Simply put streambuf 
	object is the buffer for the stream. When we read data from a stream, we don’t read it directly 
	from the source, but instead, we read it from the buffer which is linked to the source. Similarly, 
	output operations are first performed on the buffer, and then the buffer is flushed (written to the physical device) 
	when needed.

	C++ allows us to set the stream buffer for any stream. So the task of redirecting the stream simply reduces to 
	changing the stream buffer associated with the stream. Thus the to redirect a Stream A to Stream B we need to do

	1)Get the stream buffer of A and store it somewhere
	2)Set the stream buffer of A to the stream buffer of B
	3)If needed reset the stream buffer of A to its previous stream buffer

	We can use the function ios::rdbuf() to perform two opeations.
	1) stream_object.rdbuf(): Returns pointer to the stream buffer of stream_object
	2) stream_object.rdbuf(streambuf * p): Sets the stream buffer to the object pointed by p



*/
// Backup streambuffers of  cout 
	std::streambuf* stream_buffer_cout = std::cout.rdbuf();
	std::streambuf* stream_buffer_cin = std::cin.rdbuf();


	std::streambuf* inbuf = std::cin.rdbuf();
	std::istream my_cin(inbuf);
	int x;
	my_cin >> x;

	std::streambuf* outbuf = std::cout.rdbuf();
	std::ostream my_cout(outbuf);
	my_cout << x<<"\n";



	std::fstream file("myfile.txt", std::ios::out);

	// Get the streambuffer of the file 
	std::streambuf* stream_buffer_file = file.rdbuf();

	// Redirect cout to file 
	std::cout.rdbuf(stream_buffer_file);

	std::cout << "This line will be written to the file" << std::endl;

	// Redirect cout back to screen 
	std::cout.rdbuf(stream_buffer_cout);
	std::cout << "This line is written to screen" << std::endl;
}


void flushExample() 
{
/*
	By default, std::cout is buffered, and the actual output is only printed once the buffer is full or some other flushing situation occurs
	(e.g. a newline in the stream). Sometimes you want to make sure that the printing happens immediately, and you need to flush manually.
	For example, suppose you want to report a progress report by printing a single dot:
	In the following example if you comment std::flush(std::cout) you won't see that for a long time, until the buffer became full
*/
	for (;;)
	{

		// performing some expensive operation
		std::size_t j;
		for (std::size_t i = 0; i < 10000000; i++)
		{
			j = 2 * i;
		}

		std::cout << '.';
		//std::flush(std::cout);
	}
}

//////////////////////////////////////  Manipulators //////////////////////////////////////

void wsExample() 
{
/*
	std::ws
	Discards leading whitespace from an input stream
	formatted input, i.e., the usual input operators using `in >> value, skip leading whitespace and stop whenever the format is filled
	unformatted input, e.g., std::getline(in, value) does not skip leading whitespace

	int age;
	std::string fullname;

	if (std::cin >> age && std::getline(std::cin, fullname)) { // BEWARE: this is NOT a Good Idea!
		 std::cout << "age=" << age << "  fullname='" << fullname << "'\n";
	 }

	for the folliwng example:
	47
	Dietmar Kühl

	It would print something like this
	age=47 fullname=''

	The use of std::cin >> std::ws skips the whitespace, in particular the newline, and carries on reading where the actual content is entered.
	The following statement read the data correctly
	if (std::cin >> age && std::getline(std::cin >> std::ws, fullname)) {
		...
	}
*/

	int age(0);
	std::string fullname;
	std::stringstream ss1("     47 \n \n mumbo jumbo");
	
	if (ss1 >> age && std::getline(ss1 , fullname))
	{
		std::cout << age << std::endl;
		std::cout << fullname << std::endl;
	}

	std::stringstream ss2("     47 \n \n mumbo jumbo");
	if (ss2 >> age && std::getline(ss2 >> std::ws, fullname))
	{
		std::cout << age << std::endl;
		std::cout << fullname << std::endl;
	}
}

void setwExample() 
{
	std::cout << std::setw(10) << 77 << std::setw(8) << 15 << std::endl;
}

void setfillExample() 
{
	//Sets c as the stream's fill character.
	std::cout << std::setfill('x') << std::setw(10) << 77 << std::setw(10) << 12 << std::endl;
}

void leftrightExample() 
{
/*
	The std::setw manipulator sets the width of a column, while std::left and std::right set the alignment of the written value
	within that column. For example, on line 6, we write the name “John Smith” to a column of width 12 and align it to the left of the column.
*/
	std::cout << std::left << std::setw(15) << "John Smith" << std::right << std::setw(7) << 23 << '\n';
	std::cout << std::left << std::setw(15) << "Sam Brown" << std::right << std::setw(7) << 8 << '\n';
}

void setprecisionExample() 
{

	double number = 3.1914534559;

	std::cout <<"default precision: " <<std::cout.precision()<< "\n";

	std::cout << number << "\n";
	std::cout << std::setprecision(1)<< number <<"\n" ;
	std::cout << std::fixed;
	std::cout << std::setprecision(7) << number << "\n";
}

void hexDecOctSetBaseShowBase()
{
	//
	std::cout << std::setbase(16) << 110<<std::endl;
	std::cout << std::hex << 110 << std::endl;
	std::cout << std::hex << std::showbase << 110 << '\n';
	
	std::cout << std::dec << 0xc1 << std::endl;
	std::cout << std::setbase(10) << 0xc1 << std::endl;
}

//////////////////////////////////////  cin, cout //////////////////////////////////////

void cinGetlineExample()
{
/*
	std::getline

	istream& getline (istream&  is, string& str, char delim);
	istream& getline (istream&  is, string& str);
	Extracts characters from is and stores them into str until the delimitation character delim is found (or the newline character, '\n'
*/
	std::stringstream ss("this is a stringstream");
	std::string my_string;
	char delim = ' ';


	while (std::getline(ss, my_string, delim))
		std::cout << my_string << std::endl;

	std::string name;
	std::cout << "Please, enter your full name: ";
	std::getline(std::cin, name);
	std::cout << "Hello, " << name << "!\n";
}

void cinExtractOperatorExample()
{
	/*
	extract operator >>
	It will read the user input (discard the white spaces or '\n' before the user input) until it encounter 
	first end of line or white space. It will also leave the '\n' in the cin object.
	If user input has a space in it, you should use getline.
	for instance here favorite food might have two words with a space in between, so the second part of the favorite food will be
	passed to the next std::cin. 

	In the following example in the line:
	std::cin >> n1;

	since it leaves the '\n' in the cin object, the line:
	std::getline(std::cin >> std::ws, key);

	would only get '\n' and finishes immediately. 
	*/

	int n1;
	std::string favouriteFood,key;

	std::getline(std::cin>>std::ws , favouriteFood);
	std::cout << favouriteFood << '\n';

	std::cin >> n1;
	std::cout << n1 << '\n'; 
			
	std::getline(std::cin >> std::ws, key);
	std::cout << key <<'\n';

}

void cinIgnoreExample() 
{


	/*
	It doesn't "throw away" something you don't need instead, it ignores the amount of characters you specify when you call it,
	up to the char you specify as a breakpoint.

	Essentially, for std::cin statements you use ignore before you do a getline call, because when a user inputs something with std::cin,
	they hit enter and a '\n' char gets into the cin buffer. Then if you use getline, it gets the newline char instead of the string you want.
	So you do a std::cin.ignore(MAX,'\n') and that should clear the buffer up to the string that you want. (The MAX is put there to skip
	over a specific amount of chars before the specified break point, in this case, the \n newline character.)
	*/

	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	//in c style programming you can call 
	//while ((getchar()) != '\n');

	/*
	If you don't specify any argument it will ignore the first character it faces
	std::cin.ignore(5); //it will discared 5 character
	std::cin.ignore(5, demiliter); //it will discared 5 character or delimiter character i.e. '\n'
	*/

}

void cinPeekExample()
{
	/*
	It just move the pointer in the stream buffer and doesn't remove anything from that
	*/
	std::cin.peek();
}

void cinPutbackExample()
{
	/*
	It putback something in the stream buffer.
	*/
}

void cinGetExample()
{
	/*
	Extracts characters from the stream, as unformatted input:
	*/

	char str[256];
	std::cout << "Enter the name of an existing text file: ";
	std::cin.get(str, 256);    // get c-string

	std::string filePath = "";
	std::ifstream is(filePath.c_str(), std::ifstream::in);     // open file

	char c;
	while (is.get(c))          // loop getting single characters
		std::cout << c;

	c = std::cin.get();
}

void coutPutExample() 
{
	std::cout.put(65);
}

void coutWriteExample()
{
	std::string message("message for write");
	std::cout.write(message.c_str(), message.size());
}

void coutformating()
{
    /*

    std::cout.setf(arg1, arg2);

                                arg1                   arg2
    left justified output       std::ios::left         std::ios::adjustfield
    right justified output      std::ios::right        std::ios::adjustfield
    Scientific notation         std::ios::scientific   std::ios::floatfield
    Fixed point notation        std::ios::fixed        std::ios::floatfield
    Decimal base                std::ios::dec          std::ios::basefield
    Octal base                  std::ios::oct          std::ios::basefield
    Hexdecimal base             std::ios::hex          std::ios::basefield
    */


    std::cout.setf(std::ios::oct,std::ios::basefield);
    std::cout<<34<<std::endl; //will show 42
    std::cout.setf(std::ios::dec,std::ios::basefield);
    std::cout<<34<<std::endl; //will show 42
    std::cout.width(10);


    std::cout.setf(std::ios::scientific,std::ios::floatfield);
    std::cout<<3.14<<std::endl;//3.140000e+00
    std::cout.setf(std::ios::fixed,std::ios::floatfield);
    std::cout<<3.14<<std::endl;//3.140000
    std::cout.precision(1);
    std::cout<<3.14<<std::endl;//3.1
    std::cout<<std::cout.flags() <<std::endl;//3.1


    std::cout.width(6);
    std::cout.fill('*');
    std::cout.unsetf(std::ios::showbase);

}

void fastIO()
{
/*
It is often recommended to use scanf/printf instead of cin/cout for a fast input and output.
However, you can still use cin/cout and achieve the same speed as scanf/printf by including the following two
lines in your main() function:
*/
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);
/*
It toggles on or off the synchronization of all the C++ standard streams with their corresponding standard C streams if
it is called before the program performs its first input or output operation.
Adding ios_base::sync_with_stdio (false); (which is true by default) before any I/O operation avoids this synchronization.
It is a static member of function of std::ios_base.

tie() is a method which simply guarantees the flushing of std::cout before std::cin accepts an input.
This is useful for interactive console programs which require the console to be updated constantly but also slows
down the program for large I/O. The NULL part just returns a NULL pointer.

It is recommended to use cout << “\n”; instead of cout << endl;. endl is slower because it forces a flushing stream,
which is usually unnecessary
*/
}


void filesystemEample()
{

    const auto FilePath {"FileToCopy"};

    // If any filepath exists
    if(std::filesystem::exists(FilePath)) {
        const auto FileSize {
             std::filesystem::file_size(FilePath)
        };

        std::filesystem::path tmpPath {"/tmp"};

        // If filepath is available or not
        if(std::filesystem::space(tmpPath)
                           .available > FileSize) {

           // Create Directory
           std::filesystem::create_directory(
                     tmpPath.append("example"));

           // Copy File to file path
           std::filesystem::copy_file(FilePath,
                                 tmpPath.append("newFile"));
        }
    }
}

int main()
{
	
}
