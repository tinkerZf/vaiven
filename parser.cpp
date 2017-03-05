#include "parser.h"

#include "ast/all.h"
#include "visitor/assignment_producer.h"

#include <vector>
using std::vector;
using vaiven::visitor::AssignmentProducer;

using namespace vaiven;

unique_ptr<ast::Node<> > Parser::parseLogicalGroup() {
  if (current->type == TOKEN_TYPE_EOF) {
    return unique_ptr<ast::Node<> >((ast::Node<>*) NULL);
  }

  if (current->type == TOKEN_TYPE_CLOSE_BRACE) next(); // for next block after {}
  if (current->type == TOKEN_TYPE_END) next(); // for next block after fn..end
  while (current->type == TOKEN_TYPE_SEMICOLON) next(); // consume semicolon

  // after next();
  if (current->type == TOKEN_TYPE_EOF) {
    return unique_ptr<ast::Node<> >((ast::Node<>*) NULL);
  }

  if (current->type == TOKEN_TYPE_FN) {
    lastLogicalGroupWasEvaluatable = false;
    return parseFuncDecl();
  }

  lastLogicalGroupWasEvaluatable = true;
  unique_ptr<ast::Statement<> > expr = parseStatement();
  return unique_ptr<ast::Node<> >(expr.release());
}

unique_ptr<ast::FuncDecl<> > Parser::parseFuncDecl() {
  if (current->type != TOKEN_TYPE_FN) {
    throw string("expected fn");
  }
  next();
  if(current->type != TOKEN_TYPE_ID) {
    throw string("expected function name");
  }
  unique_ptr<StringToken> nametok(static_cast<StringToken*>(current.release()));
  string name = nametok->lexeme;
  next();
  vector<string> args;
  if (current->type == TOKEN_TYPE_OF) {
    next();
    while(current->type == TOKEN_TYPE_ID) {
      unique_ptr<StringToken> idtok(static_cast<StringToken*>(current.release()));
      next();
      args.push_back(idtok->lexeme);
      if (current->type != TOKEN_TYPE_COMMA) {
        break;
      }
      next();
    }
  }

  if (current->type != TOKEN_TYPE_IS) {
    throw string("expected is");
  }

  next();
  vector<unique_ptr<ast::Statement<> > > stmts;
  while (current->type != TOKEN_TYPE_EOF
      && current->type != TOKEN_TYPE_END) {
    stmts.push_back(parseStatement());
    next(); // statements don't consume their final token
  }

  if (current->type != TOKEN_TYPE_END) {
    throw string("missing close brace");
  }

  return unique_ptr<ast::FuncDecl<> >(new ast::FuncDecl<>(name, args, std::move(stmts)));
}

unique_ptr<ast::Statement<> > Parser::parseStatement() {
  if (current->type == TOKEN_TYPE_OPEN_BRACE) {
    return unique_ptr<ast::Statement<> > (parseBlock().release());
  } else if (current->type == TOKEN_TYPE_VAR) {
    return unique_ptr<ast::Statement<> > (parseVarDecl().release());
  } else if (current->type == TOKEN_TYPE_IF) {
    return unique_ptr<ast::Statement<> > (parseIfStatement().release());
  } else if (current->type == TOKEN_TYPE_RET) {
    return unique_ptr<ast::Statement<> > (parseReturnStatement().release());
  } else {
    return unique_ptr<ast::Statement<> > (parseExpressionStatement().release());
  }
}

unique_ptr<ast::VarDecl<> > Parser::parseVarDecl() {
  next();
  if(current->type != TOKEN_TYPE_ID) {
    throw string("expected var name");
  }
  unique_ptr<StringToken> nametok(static_cast<StringToken*>(current.release()));
  string name = nametok->lexeme;
  next();
  if(current->type != TOKEN_TYPE_EQ) {
    throw string("expected =");
  }
  next();
  unique_ptr<ast::Expression<> > initializer = parseExpression();
  if (current->type != TOKEN_TYPE_SEMICOLON) {
    throw string("missing end semicolon");
  }

  return unique_ptr<ast::VarDecl<> >(new ast::VarDecl<>(name, std::move(initializer)));
}

unique_ptr<ast::Block<> > Parser::parseBlock() {
  next();
  vector<unique_ptr<ast::Statement<> > > stmts;
  while (current->type != TOKEN_TYPE_EOF
      && current->type != TOKEN_TYPE_CLOSE_BRACE) {
    stmts.push_back(parseStatement());

    next(); // statements don't consume their final token
  }

  if (current->type != TOKEN_TYPE_CLOSE_BRACE) {
    throw string("missing close brace");
  }


  return unique_ptr<ast::Block<> >(new ast::Block<>(std::move(stmts)));
}

unique_ptr<ast::IfStatement<> > Parser::parseIfStatement() {
  next();
  unique_ptr<ast::Expression<> > condition(parseExpression());

  if (current->type != TOKEN_TYPE_DO) {
    throw string("missing end");
  }
  next();

  vector<unique_ptr<ast::Statement<> > > trueStmts;
  vector<unique_ptr<ast::Statement<> > > falseStmts;
  while (current->type != TOKEN_TYPE_EOF
      && current->type != TOKEN_TYPE_ELSE
      && current->type != TOKEN_TYPE_END) {
    trueStmts.push_back(parseStatement());

    next(); // statements don't consume their final token
  }

  if (current->type == TOKEN_TYPE_ELSE) {
    next();
    while (current->type != TOKEN_TYPE_EOF
        && current->type != TOKEN_TYPE_END) {
      falseStmts.push_back(parseStatement());

      next(); // statements don't consume their final token
    }
  }

  if (current->type != TOKEN_TYPE_END) {
    throw string("missing end");
  }

  return unique_ptr<ast::IfStatement<> >(new ast::IfStatement<>(
      std::move(condition),
      std::move(trueStmts),
      std::move(falseStmts)));
}

unique_ptr<ast::ReturnStatement<> > Parser::parseReturnStatement() {
  next();
  unique_ptr<ast::ReturnStatement<> > stmt(new ast::ReturnStatement<>(parseExpression()));

  if (current->type != TOKEN_TYPE_SEMICOLON) {
    throw string("missing end semicolon");
  }
  
  return std::move(stmt);
}

unique_ptr<ast::ExpressionStatement<> > Parser::parseExpressionStatement() {
  unique_ptr<ast::ExpressionStatement<> > stmt(new ast::ExpressionStatement<>(parseExpression()));

  if (current->type != TOKEN_TYPE_SEMICOLON) {
    throw string("missing end semicolon");
  }
  
  return std::move(stmt);
}


unique_ptr<ast::Expression<> > Parser::parseExpression() {
  unique_ptr<ast::Expression<> > lhs(parseAssignmentExpression());
  return std::move(lhs);
}

unique_ptr<ast::Expression<> > Parser::parseAssignmentExpression() {
  unique_ptr<ast::Expression<> > lhs(parseEqualityExpression());

  while (current->type != TOKEN_TYPE_SEMICOLON
      && current->type != TOKEN_TYPE_CLOSE_PAREN
      && current->type != TOKEN_TYPE_COMMA
      && current->type != TOKEN_TYPE_DO
      && current->type != TOKEN_TYPE_EOF) {

    if (current->type == TOKEN_TYPE_EQ) {
      next();
      unique_ptr<ast::Expression<> > rhs = parseAssignmentExpression();
      AssignmentProducer assignmentProducer(std::move(rhs));
      lhs->accept(assignmentProducer);
      return std::move(assignmentProducer.result);
    } else {
      // error
      throw string("blah");
    }
  }

  return std::move(lhs);
}

unique_ptr<ast::Expression<> > Parser::parseEqualityExpression() {
  unique_ptr<ast::Expression<> > acc(parseComparisonExpression());

  while (current->type != TOKEN_TYPE_SEMICOLON
      && current->type != TOKEN_TYPE_CLOSE_PAREN
      && current->type != TOKEN_TYPE_COMMA
      && current->type != TOKEN_TYPE_DO
      && current->type != TOKEN_TYPE_EQ
      && current->type != TOKEN_TYPE_EOF) {

    if (current->type == TOKEN_TYPE_EQEQ) {
      next();
      acc = unique_ptr<ast::Expression<> >(new ast::EqualityExpression<> (
          std::move(acc), parseComparisonExpression()));
    } else if (current->type == TOKEN_TYPE_BANGEQ) {
      next();
      acc = unique_ptr<ast::Expression<> >(new ast::InequalityExpression<> (
          std::move(acc), parseComparisonExpression()));
    } else {
      // error
      throw string("blah");
    }
  }

  return std::move(acc);
}

unique_ptr<ast::Expression<> > Parser::parseComparisonExpression() {
  unique_ptr<ast::Expression<> > acc(parseAddSubExpression());

  while (current->type != TOKEN_TYPE_SEMICOLON
      && current->type != TOKEN_TYPE_CLOSE_PAREN
      && current->type != TOKEN_TYPE_COMMA
      && current->type != TOKEN_TYPE_DO
      && current->type != TOKEN_TYPE_EQ
      && current->type != TOKEN_TYPE_EQEQ
      && current->type != TOKEN_TYPE_BANGEQ
      && current->type != TOKEN_TYPE_EOF) {

    if (current->type == TOKEN_TYPE_GT) {
      next();
      acc = unique_ptr<ast::Expression<> >(new ast::GtExpression<> (
          std::move(acc), parseAddSubExpression()));
    } else if (current->type == TOKEN_TYPE_GTE) {
      next();
      acc = unique_ptr<ast::Expression<> >(new ast::GteExpression<> (
          std::move(acc), parseAddSubExpression()));
    } else if (current->type == TOKEN_TYPE_LT) {
      next();
      acc = unique_ptr<ast::Expression<> >(new ast::LtExpression<> (
          std::move(acc), parseAddSubExpression()));
    } else if (current->type == TOKEN_TYPE_LTE) {
      next();
      acc = unique_ptr<ast::Expression<> >(new ast::LteExpression<> (
          std::move(acc), parseAddSubExpression()));
    } else {
      // error
      throw string("blah");
    }
  }

  return std::move(acc);
}

unique_ptr<ast::Expression<> > Parser::parseAddSubExpression() {
  unique_ptr<ast::Expression<> > acc(parseDivMulExpression());

  while (current->type != TOKEN_TYPE_SEMICOLON
      && current->type != TOKEN_TYPE_CLOSE_PAREN
      && current->type != TOKEN_TYPE_COMMA
      && current->type != TOKEN_TYPE_DO
      && current->type != TOKEN_TYPE_EQ
      && current->type != TOKEN_TYPE_EQEQ
      && current->type != TOKEN_TYPE_BANGEQ
      && current->type != TOKEN_TYPE_GT
      && current->type != TOKEN_TYPE_GTE
      && current->type != TOKEN_TYPE_LT
      && current->type != TOKEN_TYPE_LTE
      && current->type != TOKEN_TYPE_EOF) {

    if (current->type == TOKEN_TYPE_PLUS) {
      next();
      acc = unique_ptr<ast::Expression<> >(new ast::AdditionExpression<> (
          std::move(acc), parseDivMulExpression()));
    } else if (current->type == TOKEN_TYPE_MINUS) {
      next();
      acc = unique_ptr<ast::Expression<> >(new ast::SubtractionExpression<> (
          std::move(acc), parseDivMulExpression()));
    } else {
      // error
      throw string("blah");
    }
  }

  return std::move(acc);

}

unique_ptr<ast::Expression<> > Parser::parseDivMulExpression() {
  unique_ptr<ast::Expression<> > acc(parseValue());

  while (current->type != TOKEN_TYPE_SEMICOLON
      && current->type != TOKEN_TYPE_PLUS
      && current->type != TOKEN_TYPE_MINUS
      && current->type != TOKEN_TYPE_MINUS
      && current->type != TOKEN_TYPE_COMMA
      && current->type != TOKEN_TYPE_CLOSE_PAREN
      && current->type != TOKEN_TYPE_DO
      && current->type != TOKEN_TYPE_EQ
      && current->type != TOKEN_TYPE_EQEQ
      && current->type != TOKEN_TYPE_BANGEQ
      && current->type != TOKEN_TYPE_GT
      && current->type != TOKEN_TYPE_GTE
      && current->type != TOKEN_TYPE_LT
      && current->type != TOKEN_TYPE_LTE
      && current->type != TOKEN_TYPE_EOF) {

    if (current->type == TOKEN_TYPE_DIVIDE) {
      next();
      acc = unique_ptr<ast::Expression<> >(new ast::DivisionExpression<> (
          std::move(acc), parseValue()));
    } else if (current->type == TOKEN_TYPE_MULTIPLY) {
      next();
      acc = unique_ptr<ast::Expression<> >(new ast::MultiplicationExpression<> (
          std::move(acc), parseValue()));
    } else {
      // error
      throw string("blah");
    }
  }

  return std::move(acc);
}

unique_ptr<ast::Expression<> > Parser::parseValue() {
  if (current->type == TOKEN_TYPE_INTEGER) {
    unique_ptr<IntegerToken> inttok(static_cast<IntegerToken*>(current.release()));
    next();
    return unique_ptr<ast::Expression<> >(new ast::IntegerExpression<> (inttok->value));
  }

  if (current->type == TOKEN_TYPE_OPEN_PAREN) {
    next();
    unique_ptr<ast::Expression<> > expr(parseExpression());
    // TODO check it actually is TYPE_CLOSE_PAREN
    next();
    return std::move(expr);
  }

  if (current->type == TOKEN_TYPE_ID) {
    unique_ptr<StringToken> idtok(static_cast<StringToken*>(current.release()));
    next();
    if (current->type == TOKEN_TYPE_OPEN_PAREN) {
      next();
      vector<unique_ptr<ast::Expression<> > > params;
      while (current->type != TOKEN_TYPE_EOF
          && current->type != TOKEN_TYPE_CLOSE_PAREN) {
        params.push_back(parseExpression());

        if (current->type != TOKEN_TYPE_COMMA) {
          break;
        }

        next();
      }

      if (current->type != TOKEN_TYPE_CLOSE_PAREN) {
        throw string("missing close paren");
      }

      next();
      return unique_ptr<ast::Expression<> >(new ast::FuncCallExpression<> (idtok->lexeme, move(params)));
    } else {
      return unique_ptr<ast::Expression<> >(new ast::VariableExpression<> (idtok->lexeme));
    }
  }

  if (current->type == TOKEN_TYPE_TRUE) {
    next();
    return unique_ptr<ast::Expression<> >(new ast::BoolLiteral<> (true));
  }

  if (current->type == TOKEN_TYPE_FALSE) {
    next();
    return unique_ptr<ast::Expression<> >(new ast::BoolLiteral<> (false));
  }

  if (current->type == TOKEN_TYPE_BANG) {
    next();
    return unique_ptr<ast::Expression<> >(new ast::NotExpression<> (parseExpression()));
  }
  
  throw string("blah");
}

void Parser::next() {
  current = tokenizer.next();
}

