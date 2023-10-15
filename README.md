# small C++ ansi utilities

> example

![Capture](https://github.com/Nathan-Morris/ansi/assets/73218530/318904cb-c64c-4e97-bf1d-5f87e26babc0)

```cpp
#include "ansi.h"

using namespace std;
using namespace ansi;

using std::cout;
using std::endl;
using std::vector;

int main(int argc, char* argv[]) {

	init();

	//
	//

	cout << fore::red << "lorem "
		<< fore::white << "ipsum dolor sit amet, "
		<< fore::blue_bright << "consectetur adipiscing elit "
		<< reset << bold << "sed do eiusmod tempor incididunt"
		<< reset << endl << endl;

	//
	//

	cout << text("lorem ", fore::red) << "ipsum dolor sit amet" << endl << endl;

	//
	//

	cout << mapped("=========([ Mapped String 1.0 ])=========")
		.map(char_t::alpha, color_t::magenta)
		.map(char_t::numeric, 0xF12711)
		.map('=', 0xF5AF19)
		<< endl << endl;

	//
	//

	const char* grad_string = R"(    
    .__________________________.
    | .___________________. |==|
    | |     Apple ][      | |  |
    | |                   | |  |
    | |                   | |  |
    | |                   | |  |
    | |                   | |  |
    | |                   | |  |
    | | ]                 | | ,|
    | !___________________! |(c|
    !_______________________!__!
    |    ___ -=      ___ -= | ,|
    | ---[_]---   ---[_]--- |(c|

    !_______________________!__!
   /                            \
  /  [][][][][][][][][][][][][]  \
 /  [][][][][][][][][][][][][][]  \
(  [][][][][____________][][][][]  )
 \ ------------------------------ /
  \______________________________/)";

	vector<rgb_t> grad_steps = {
		color_t::red,
		color_t::yellow,
		color_t::green,
		color_t::cyan,
		color_t::blue,
		color_t::white
	};

	cout << gradient(grad_string, grad_steps) << endl << endl;

	//
	//
}
```


