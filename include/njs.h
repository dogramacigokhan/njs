#ifndef NJS_HEADER
#define NJS_HEADER
#include<stack>
#include<string>
#include<iostream>
#define _STACK(S) ((stack<Data>*)((S).data))
using namespace std;
enum DataType { STACK, INTEGER, DOUBLE, TEXT, FUNCTION, IF, THEN, ELSE };
class Data{
private:
	int* refcount;
	void destroy(){
		switch (type){
		case DOUBLE: delete (double*)data; break;
		case INTEGER: delete (int*)data; break;
		case TEXT: delete (string*)data; break;
		case STACK: delete (stack<Data>*)data; break;
		}
		delete refcount;
	}
public:
	void* data; DataType type;
	Data(void* data_, DataType type_) : data(data_), type(type_){
		refcount = new int;		
		*refcount = 1;
	}
	Data(const Data& d){
		refcount = d.refcount;
		(*refcount)++;
		data = d.data;
		type = d.type;
	}
	Data& operator=(const Data& d){
		(*refcount)--;
		if (*refcount == 0){
			destroy();
		}
		refcount = d.refcount;
		(*refcount)++;
		data = d.data;
		type = d.type;
		return *this;
	}
	~Data(){
		(*refcount)--;
		if (*refcount == 0){ destroy(); }
	}
};

void njs_private_func_eval(stack<Data>& s);

void njs_private_func_if_helper(stack<Data>& this_, int njs_temp_bool)
{
	this_.pop();	
	if (njs_temp_bool){
		while (!this_.empty() && this_.top().type != THEN){
			this_.pop();
		}
		if (!this_.empty()){
			this_.pop();
		}
		stack<Data> this__temp;
		while (!this_.empty() && this_.top().type != ELSE){
			this__temp.push(this_.top());
			this_.pop();
		}
		while (!this_.empty()){
			this_.pop();
		}
		while (!this__temp.empty()){
			this_.push(this__temp.top());
			this__temp.pop();
		}
	}
	else{
		while (!this_.empty() && this_.top().type != ELSE){
			this_.pop();
		}if (!this_.empty()){ this_.pop(); }
	}
}

void njs_private_func_if(stack<Data>& this_){
	this_.pop();
	if (!this_.empty()){
		Data njs_temp_data = this_.top();		
		if (njs_temp_data.type == INTEGER){
			njs_private_func_if_helper(this_, *(int*)njs_temp_data.data);
		}
		else 
		{
			njs_private_func_eval(this_);
			Data njs_temp_data = this_.top();
			if (njs_temp_data.type == INTEGER){
				njs_private_func_if_helper(this_, *(int*)njs_temp_data.data);
			}
		}
	}
}
void njs_private_func_deepCopy(stack<Data>& stack1, stack<Data>& stack2){
	stack<Data> tempStack, srcStack; while (!stack1.empty()){
		Data njs_temp_data = stack1.top(); stack1.pop(); Data copyData(NULL, IF); switch (njs_temp_data.type){
		case INTEGER: copyData.data = new int(*(int*)njs_temp_data.data); copyData.type = INTEGER; break;
		case DOUBLE: copyData.data = new double(*(double*)njs_temp_data.data); copyData.type = DOUBLE; break;
		case TEXT: copyData.data = new string(*(string*)njs_temp_data.data); copyData.type = TEXT; break;
		case STACK: copyData.data = new stack<Data>(*(stack<Data>*)njs_temp_data.data); copyData.type = STACK; break;
		default: copyData.data = njs_temp_data.data; copyData.type = njs_temp_data.type; break;
		} tempStack.push(copyData); srcStack.push(njs_temp_data);
	}
	while (!tempStack.empty()){ stack2.push(tempStack.top()); tempStack.pop(); stack1.push(srcStack.top()); srcStack.pop(); }
}
void njs_private_func_eval(stack<Data>& s){
	if (!s.empty()){
		if (s.top().type == FUNCTION){
			void(*njs_function)(stack<Data>&) = (void(*)(stack<Data>&))s.top().data; s.pop(); njs_function(s);
		}
		else if (s.top().type == IF){
			njs_private_func_if(s);
		}
	}
}
#endif