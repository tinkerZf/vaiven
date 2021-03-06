#include "interpreter.h"

#include <iostream>

#include "../ast/all.h"
#include "../runtime_error.h"
#include "../std.h"

using std::cout;

using namespace vaiven;
using namespace vaiven::visitor;

//int Interpreter::interpret(Node<>& root, vector<Value> args, map<string, int>* variablesMap) {
Value Interpreter::interpret(Node<>& root) {
  //for(vector<int>::iterator it = args.begin(); it != args.end(); ++it) {
  //  stack.push(*it);
  //}
  //this->variablesMap = variablesMap;

  root.accept(*this);

  if (stack.size()) {
    return stack.top();
  } else {
    return Value();
  }

  //for(int i = 0; i < args.size(); ++i) {
  //  stack.pop();
  //}
  //stack.pop();

  //return val;
}

void Interpreter::visitIfStatement(IfStatement<>& stmt) {
  stmt.condition->accept(*this);
  Value condVal = stack.top(); stack.pop();
  if (!condVal.isBool()) {
    expectedBool();
  }

  if (condVal.getBool()) {
    ScopeFrame<Value> trueScope(scope);
    for(vector<unique_ptr<Statement<> > >::iterator it = stmt.trueStatements.begin();
        it != stmt.trueStatements.end();
        ++it) {
      if (stack.size()) {
        stack.pop();
      }
      (*it)->accept(*this);
    }
  } else {
    ScopeFrame<Value> falseScope(scope);
    for(vector<unique_ptr<Statement<> > >::iterator it = stmt.falseStatements.begin();
        it != stmt.falseStatements.end();
        ++it) {
      if (stack.size()) {
        stack.pop();
      }
      (*it)->accept(*this);
    }
  }
}

void Interpreter::visitForCondition(ForCondition<>& stmt) {
  do {
    stmt.condition->accept(*this);
    Value condVal = stack.top(); stack.pop();
    if (!condVal.isBool()) {
      expectedBool();
    }

    if (!condVal.getBool()) {
      break;
    }

    ScopeFrame<Value> subScope(scope);
    bool first = true;
    for(vector<unique_ptr<Statement<> > >::iterator it = stmt.statements.begin();
        it != stmt.statements.end();
        ++it) {
      (*it)->accept(*this);
      if (stack.size()) {
        stack.pop();
      }
    }
  } while(true);
}

void Interpreter::visitReturnStatement(ReturnStatement<>& stmt) {
  stmt.expr->accept(*this);
  throw(stack.top());
}

void Interpreter::visitVarDecl(VarDecl<>& varDecl) {
  varDecl.expr->accept(*this);
  if (!scope.contains(varDecl.varname)) {
    scope.put(varDecl.varname, stack.top());
    stack.pop();
  } else {
    duplicateVar();
  }
}

void Interpreter::visitListLiteralExpression(ListLiteralExpression<>& expr) {
  GcableList* list = heap.newList();
  list->list.reserve(expr.items.size());
  for(vector<unique_ptr<Expression<> > >::iterator it = expr.items.begin();
      it != expr.items.end();
      ++it) {
    (*it)->accept(*this);
    list->list.push_back(stack.top());
    stack.pop();
  }

  stack.push(list);
}

void Interpreter::visitDynamicAccessExpression(DynamicAccessExpression<>& expr) {
  expr.subject->accept(*this);
  expr.property->accept(*this);
  Value property = stack.top(); stack.pop();
  Value subject = stack.top(); stack.pop();

  stack.push(vaiven::get(subject, property));
}

void Interpreter::visitDynamicStoreExpression(DynamicStoreExpression<>& expr) {
  expr.subject->accept(*this);
  expr.property->accept(*this);
  expr.rhs->accept(*this);
  Value rhs = stack.top(); stack.pop();
  Value property = stack.top(); stack.pop();
  Value subject = stack.top(); stack.pop();

  switch (expr.preAssignmentOp) {
    case ast::kPreAssignmentOpNone:
      vaiven::set(subject, property, rhs);
      stack.push(rhs);
      break;

    case ast::kPreAssignmentOpAdd: {
      Value result = vaiven::add(vaiven::get(subject, property), rhs);
      vaiven::set(subject, property, result);
      stack.push(result);
      break;
    }
    case ast::kPreAssignmentOpSub: {
      Value result = vaiven::sub(vaiven::get(subject, property), rhs);
      vaiven::set(subject, property, result);
      stack.push(result);
      break;
    }
    case ast::kPreAssignmentOpMul: {
      Value result = vaiven::mul(vaiven::get(subject, property), rhs);
      vaiven::set(subject, property, result);
      stack.push(result);
      break;
    }
    case ast::kPreAssignmentOpDiv: {
      Value result = vaiven::div(vaiven::get(subject, property), rhs);
      vaiven::set(subject, property, result);
      stack.push(result);
      break;
    }
  }
}

void Interpreter::visitStaticAccessExpression(StaticAccessExpression<>& expr) {
  expr.subject->accept(*this);
  Value subject = stack.top(); stack.pop();

  stack.push(vaiven::objectAccessChecked(subject, expr.property));
}

void Interpreter::visitStaticStoreExpression(StaticStoreExpression<>& expr) {
  expr.subject->accept(*this);
  expr.rhs->accept(*this);
  Value rhs = stack.top(); stack.pop();
  Value subject = stack.top(); stack.pop();

  switch (expr.preAssignmentOp) {
    case ast::kPreAssignmentOpNone:
      vaiven::objectStoreChecked(subject, expr.property, rhs);
      stack.push(rhs);
      break;

    case ast::kPreAssignmentOpAdd: {
      Value result = vaiven::add(vaiven::objectAccessChecked(subject, expr.property), rhs);
      vaiven::objectStoreUnchecked((GcableObject*) subject.getPtr(), expr.property, result);
      stack.push(result);
      break;
    }
    case ast::kPreAssignmentOpSub: {
      Value result = vaiven::sub(vaiven::objectAccessChecked(subject, expr.property), rhs);
      vaiven::objectStoreUnchecked((GcableObject*) subject.getPtr(), expr.property, result);
      stack.push(result);
      break;
    }
    case ast::kPreAssignmentOpMul: {
      Value result = vaiven::mul(vaiven::objectAccessChecked(subject, expr.property), rhs);
      vaiven::objectStoreUnchecked((GcableObject*) subject.getPtr(), expr.property, result);
      stack.push(result);
      break;
    }
    case ast::kPreAssignmentOpDiv: {
      Value result = vaiven::div(vaiven::objectAccessChecked(subject, expr.property), rhs);
      vaiven::objectStoreUnchecked((GcableObject*) subject.getPtr(), expr.property, result);
      stack.push(result);
      break;
    }
  }
}

void Interpreter::visitFuncCallExpression(FuncCallExpression<>& expr) {
  if (funcs.funcs.find(expr.name) == funcs.funcs.end()) {
    noSuchFunction();
  }

  // TODO check arg counts
  OverkillFunc fptr = funcs.funcs[expr.name]->fptr;
  for(vector<unique_ptr<Expression<> > >::iterator it = expr.parameters.begin();
      it != expr.parameters.end();
      ++it) {
    (*it)->accept(*this);
  }

  // make uninitialized pointers to trick the optimizer out of
  // supply all args except when necessary!
  //void* uninitialized;
  //Value eight(uninitialized);
  //Value seven(uninitialized);
  //Value six(uninitialized);
  //Value five(uninitialized);
  //Value four(uninitialized);
  //Value three(uninitialized);
  //Value two(uninitialized);
  //Value one(uninitialized);
  Value eight;
  Value seven;
  Value six;
  Value five;
  Value four;
  Value three;
  Value two;
  Value one;
  switch (expr.parameters.size()) {
    case 8:
      eight = stack.top(); stack.pop();
    case 7:
      seven = stack.top(); stack.pop();
    case 6:
      six = stack.top(); stack.pop();
    case 5:
      five = stack.top(); stack.pop();
    case 4:
      four = stack.top(); stack.pop();
    case 3:
      three = stack.top(); stack.pop();
    case 2:
      two = stack.top(); stack.pop();
    case 1:
      one = stack.top(); stack.pop();
    case 0: break;
    default:
      throw "crap, too many args for my crappy code!";
  }
  
  stack.push(fptr(one, two, three, four, five, six, seven, eight));
}

void Interpreter::visitFuncDecl(FuncDecl<>& decl) {
  // nothing
}

void Interpreter::visitExpressionStatement(ExpressionStatement<>& stmt) {
  stmt.expr->accept(*this);
}

void Interpreter::visitBlock(Block<>& block) {
  for(vector<unique_ptr<Statement<> > >::iterator it = block.statements.begin();
      it != block.statements.end();
      ++it) {
    if (it != block.statements.begin()) {
      stack.pop();
    }
    (*it)->accept(*this);
  }
}

void Interpreter::visitAssignmentExpression(AssignmentExpression<>& expr) {
  expr.expr->accept(*this);
  if (scope.contains(expr.varname)) {
    switch (expr.preAssignmentOp) {
      case ast::kPreAssignmentOpNone:
        scope.replace(expr.varname, stack.top());
        break;
      case ast::kPreAssignmentOpAdd: {
        Value left = scope.get(expr.varname);
        Value right = stack.top();
        Value result = vaiven::add(left, right);
        scope.replace(expr.varname, result);
        stack.pop();
        stack.push(result);
        break;
      }
      case ast::kPreAssignmentOpSub: {
        Value left = scope.get(expr.varname);
        Value right = stack.top();
        Value result = vaiven::sub(left, right);
        scope.replace(expr.varname, result);
        stack.pop();
        stack.push(result);
        break;
      }
      case ast::kPreAssignmentOpMul: {
        Value left = scope.get(expr.varname);
        Value right = stack.top();
        Value result = vaiven::mul(left, right);
        scope.replace(expr.varname, result);
        stack.pop();
        stack.push(result);
        break;
      }
      case ast::kPreAssignmentOpDiv: {
        Value left = scope.get(expr.varname);
        Value right = stack.top();
        Value result = vaiven::div(left, right);
        scope.replace(expr.varname, result);
        stack.pop();
        stack.push(result);
        break;
      }
    }
  } else {
    noSuchVar();
  }
}

void Interpreter::visitAdditionExpression(AdditionExpression<>& expr) {
  expr.left->accept(*this);
  expr.right->accept(*this);
  Value right = stack.top(); stack.pop();
  Value left = stack.top(); stack.pop();
  stack.push(add(left, right));
}
void Interpreter::visitSubtractionExpression(SubtractionExpression<>& expr) {
  expr.left->accept(*this);
  expr.right->accept(*this);
  Value right = stack.top(); stack.pop();
  Value left = stack.top(); stack.pop();
  stack.push(sub(left, right));
}
void Interpreter::visitMultiplicationExpression(MultiplicationExpression<>& expr) {
  expr.left->accept(*this);
  expr.right->accept(*this);
  Value right = stack.top(); stack.pop();
  Value left = stack.top(); stack.pop();
  stack.push(mul(left, right));
}
void Interpreter::visitDivisionExpression(DivisionExpression<>& expr) {
  expr.left->accept(*this);
  expr.right->accept(*this);
  Value right = stack.top(); stack.pop();
  Value left = stack.top(); stack.pop();
  stack.push(div(left, right));
}
void Interpreter::visitIntegerExpression(IntegerExpression<>& expr) {
  stack.push(Value(expr.value));
}

void Interpreter::visitDoubleExpression(DoubleExpression<>& expr) {
  stack.push(Value(expr.value));
}

void Interpreter::visitStringExpression(StringExpression<>& expr) {
  stack.push(Value(expr.value));
}

void Interpreter::visitVariableExpression(VariableExpression<>& expr) {
  if (scope.contains(expr.id)) {
    Value val = scope.get(expr.id);
    stack.push(val);
  } else {
    noSuchVar();
  }
}

void Interpreter::visitBoolLiteral(BoolLiteral<>& expr) {
  stack.push(Value(expr.value));
}

void Interpreter::visitVoidLiteral(VoidLiteral<>& expr) {
  stack.push(Value());
}

void Interpreter::visitNotExpression(NotExpression<>& expr) {
  expr.expr->accept(*this);
  Value val = stack.top(); stack.pop();
  if (!val.isBool()) {
    expectedBool();
  }

  stack.push(Value(!val.getBool()));
}

void Interpreter::visitInequalityExpression(InequalityExpression<>& expr) {
  expr.left->accept(*this);
  expr.right->accept(*this);
  Value right = stack.top(); stack.pop();
  Value left = stack.top(); stack.pop();
  stack.push(Value(!cmp(left, right).getBool()));
}

void Interpreter::visitEqualityExpression(EqualityExpression<>& expr) {
  expr.left->accept(*this);
  expr.right->accept(*this);
  Value right = stack.top(); stack.pop();
  Value left = stack.top(); stack.pop();
  stack.push(cmp(left, right));
}

void Interpreter::visitGtExpression(GtExpression<>& expr) {
  expr.left->accept(*this);
  expr.right->accept(*this);
  Value right = stack.top(); stack.pop();
  Value left = stack.top(); stack.pop();
  stack.push(gt(left, right));
}

void Interpreter::visitGteExpression(GteExpression<>& expr) {
  expr.left->accept(*this);
  expr.right->accept(*this);
  Value right = stack.top(); stack.pop();
  Value left = stack.top(); stack.pop();
  stack.push(gte(left, right));
}

void Interpreter::visitLtExpression(LtExpression<>& expr) {
  expr.left->accept(*this);
  expr.right->accept(*this);
  Value right = stack.top(); stack.pop();
  Value left = stack.top(); stack.pop();
  stack.push(lt(left, right));
}

void Interpreter::visitLteExpression(LteExpression<>& expr) {
  expr.left->accept(*this);
  expr.right->accept(*this);
  Value right = stack.top(); stack.pop();
  Value left = stack.top(); stack.pop();
  stack.push(lte(left, right));
}
