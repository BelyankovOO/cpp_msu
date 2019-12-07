#include <cstdio>
#include <iostream>
#include <cmath>
#include <any>
#include <map>
#include <vector>
#include <memory>
#include <string>

class Function {
public:
	virtual ~Function(){}
	virtual double operator()(double x) const = 0;
	virtual double GetDeriv(double x) const = 0;
	virtual std::string ToString() const = 0;
};

class Const: public Function{
	int _const;
public:
	Const(const std::any& value): _const(std::any_cast<int>(value)) {}
	double operator()(double x) const override{return _const;}
	double GetDeriv(double x) const override{return 0.0;}
	std::string ToString() const override{return std::to_string(_const);}
};

class Ident: public Function{
public:	
	Ident(const std::any& value) {}
	double operator()(double x) const override{return x;}
	double GetDeriv(double x) const override{return 1.;}
	std::string ToString() const override{return std::string("x");}
};

class Power: public Function{
	int _power;
public:	
	Power(const std::any& value): _power(std::any_cast<int>(value)) {}
	double operator()(double x) const override{return pow(x, _power);}
	double GetDeriv(double x) const override{return _power * pow(x, _power - 1);}
	std::string ToString() const override{return std::string("x^") + std::to_string(_power);}
};

class Exp: public Function{
	int _power;
public: 
	Exp(const std::any& value): _power(std::any_cast<int>(value)) {}
	double operator()(double x) const override{return exp(_power*x);}
	double GetDeriv(double x) const override{return _power*exp(_power*x);}
	std::string ToString() const override{return std::string("e^") + std::to_string(_power) + std::string("x");}	
};

class Polynomial: public Function{
	std::vector<int> _poly;
public:
	Polynomial(const std::any& value): _poly(std::any_cast<std::vector<int>>(value)) {}
	double operator()(double x) const override{
		double polynom = 0.0;
		for(int i=0; i<_poly.size(); i++){
			polynom = polynom + _poly[i] * pow(x, i);
		}
		return polynom;
	}
	double GetDeriv(double x) const override{
		double polynom = 0.0;
		for(int i=1; i<_poly.size(); i++){
			polynom = polynom + _poly[i]*i*pow(x, i-1);
		}
		return polynom;
	}
	std::string ToString() const override{
		std::string polynom = std::to_string(_poly[0]);
		for(int i=1; i<_poly.size(); i++){
			polynom = polynom + (std::string("+") + std::to_string(_poly[i]) + std::string("x^") + std::to_string(i));
		}
		return polynom;
	}
};	

class FunctionPlus: public Function{
	std::shared_ptr<Function> a,b;
public:
	FunctionPlus(std::any& a, std::any& b): a(std::any_cast<std::shared_ptr<Function>>(a)), b(std::any_cast<std::shared_ptr<Function>>(b)) {}
	double operator()(double x) const override{return (*a)(x) + (*b)(x);}
	double GetDeriv(double x) const override{return a->GetDeriv(x) + b->GetDeriv(x);}
	std::string ToString() const override{return a->ToString() + "+" + b->ToString();}
};

class FunctionMinus: public Function{
	std::shared_ptr<Function> a,b;
public:
	FunctionMinus(std::any& a, std::any& b): a(std::any_cast<std::shared_ptr<Function>>(a)), b(std::any_cast<std::shared_ptr<Function>>(b)) {}
	double operator()(double x) const override{return (*a)(x) - (*b)(x);}
	double GetDeriv(double x) const override{return a->GetDeriv(x) - b->GetDeriv(x);}
	std::string ToString() const override{return a->ToString() + "-" + b->ToString();}
};

class FunctionMultiply: public Function{
	std::shared_ptr<Function> a,b;
public:
	FunctionMultiply(std::any& a, std::any& b): a(std::any_cast<std::shared_ptr<Function>>(a)), b(std::any_cast<std::shared_ptr<Function>>(b)) {}
	double operator()(double x) const override{return (*a)(x) * (*b)(x);}
	double GetDeriv(double x) const override{return a->GetDeriv(x)*(*b)(x) + (*a)(x)*b->GetDeriv(x);}
	std::string ToString() const override{return a->ToString() + "*" + b->ToString();}
};

class FunctionDivide: public Function{
	std::shared_ptr<Function> a,b;
public:
	FunctionDivide(std::any& a, std::any& b): a(std::any_cast<std::shared_ptr<Function>>(a)), b(std::any_cast<std::shared_ptr<Function>>(b)) {}
	double operator()(double x) const override{return (*a)(x) / (*b)(x);}
	double GetDeriv(double x) const override{return a->GetDeriv(x)*(*b)(x) - (*a)(x)*b->GetDeriv(x) / pow((*a)(x),2);}
	std::string ToString() const override{return a->ToString() + "/" + b->ToString();}
};

std::shared_ptr<Function> operator+(std::any a, std::any b){
	try{
		std::any_cast<std::shared_ptr<Function>>(a);
		std::any_cast<std::shared_ptr<Function>>(b);
	} catch(const std::bad_any_cast& error) {
		throw std::logic_error(error.what());
	}
	return std::make_shared<FunctionPlus>(a,b);
}	

std::shared_ptr<Function> operator-(std::any a, std::any b){
	try{
		std::any_cast<std::shared_ptr<Function>>(a);
		std::any_cast<std::shared_ptr<Function>>(b);
	} catch(const std::bad_any_cast& error) {
		throw std::logic_error(error.what());
	}
	return std::make_shared<FunctionMinus>(a,b);
}	

std::shared_ptr<Function> operator*(std::any a, std::any b){
	try{
		std::any_cast<std::shared_ptr<Function>>(a);
		std::any_cast<std::shared_ptr<Function>>(b);
	} catch(const std::bad_any_cast& error) {
		throw std::logic_error(error.what());
	}
	return std::make_shared<FunctionMultiply>(a,b);
}

std::shared_ptr<Function> operator/(std::any a, std::any b){
	try{
		std::any_cast<std::shared_ptr<Function>>(a);
		std::any_cast<std::shared_ptr<Function>>(b);
	} catch(const std::bad_any_cast& error) {
		throw std::logic_error(error.what());
	}
	return std::make_shared<FunctionDivide>(a,b);
} 

double NewtonMethod(const std::shared_ptr<Function> f, double x0, int iter, double eps){
	int step = 0;	
	do{
	x0 = x0 - (*f)(x0)/f->GetDeriv(x0);
	step++;
	} while(fabs((*f)(x0)) > eps && step < iter);
	return x0;
}

class TFactory {
    class TImpl;
    std::unique_ptr<const TImpl> Impl;
public:
    TFactory();
    ~TFactory();
    std::shared_ptr<Function> CreateObject(const std::string& name, const std::any& value = nullptr) const;
};

class TFactory::TImpl{
	class ICreator{
	public:
		virtual ~ICreator(){}
		virtual std::shared_ptr<Function> Create(const std::any& value) const = 0;
	};
	using TCreatorPtr = std::shared_ptr<ICreator>;
	using TRegisteredCreators = std::map<std::string, TCreatorPtr>;
	TRegisteredCreators RegisteredCreators;
public: 
	template <class TCurrentObject>
    class TCreator : public ICreator {
        std::shared_ptr<Function> Create(const std::any& value) const override {
            return std::make_unique<TCurrentObject>(value);
        }
    };

    TImpl() {RegisterAll();}

    template <typename T>
    void RegisterCreator(const std::string& name) {
        RegisteredCreators[name] = std::make_shared<TCreator<T>>();
    }

    void RegisterAll() {
    	RegisterCreator<Const>("const");
        RegisterCreator<Ident>("ident");
        RegisterCreator<Power>("power");
        RegisterCreator<Exp>("exp");
        RegisterCreator<Polynomial>("polynomial");
    }

     std::shared_ptr<Function> CreateObject(const std::string& name, const std::any& value) const {
        auto creator = RegisteredCreators.find(name);
        if (creator == RegisteredCreators.end())
            return nullptr;
        return creator->second->Create(value);
    }
}; 

TFactory::TFactory() : Impl(std::make_unique<TFactory::TImpl>()) {}
TFactory::~TFactory() {}

std::shared_ptr<Function> TFactory::CreateObject(const std::string& name, const std::any& value) const {
    return Impl->CreateObject(name, value);
} 