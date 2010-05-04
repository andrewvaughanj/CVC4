/*********************                                                        */
/** input.h
 ** Original author: cconway
 ** Major contributors: none
 ** Minor contributors (to current version): none
 ** This file is part of the CVC4 prototype.
 ** Copyright (c) 2009, 2010  The Analysis of Computer Systems Group (ACSys)
 ** Courant Institute of Mathematical Sciences
 ** New York University
 ** See the file COPYING in the top-level source directory for licensing
 ** information.
 **
 ** Base for ANTLR parser classes.
 **/

#include "cvc4parser_private.h"

#ifndef __CVC4__PARSER__ANTLR_INPUT_H
#define __CVC4__PARSER__ANTLR_INPUT_H

#include <antlr3.h>
#include <iostream>
#include <string>
#include <vector>

#include "input.h"
#include "parser_options.h"
#include "parser_exception.h"
#include "expr/expr.h"
#include "expr/expr_manager.h"
#include "util/Assert.h"

namespace CVC4 {

class Command;
class Type;
class FunctionType;

namespace parser {

/** Wrapper around an ANTLR3 input stream. */
class AntlrInputStream : public InputStream {
  pANTLR3_INPUT_STREAM d_input;

  AntlrInputStream(std::string name,pANTLR3_INPUT_STREAM input);

  /* This is private and unimplemented, because you should never use it. */
  AntlrInputStream(const AntlrInputStream& inputStream);

  /* This is private and unimplemented, because you should never use it. */
  AntlrInputStream& operator=(const AntlrInputStream& inputStream);

public:

  virtual ~AntlrInputStream();

  pANTLR3_INPUT_STREAM getAntlr3InputStream() const;

  /** Create a file input.
   *
   * @param filename the path of the file to read
   * @param useMmap <code>true</code> if the input should use memory-mapped I/O; otherwise, the
   * input will use the standard ANTLR3 I/O implementation.
   */
  static AntlrInputStream* newFileInputStream(const std::string& name, bool useMmap = false)
    throw (InputStreamException);

  /** Create an input from an istream. */
  // AntlrInputStream newInputStream(std::istream& input, const std::string& name);

  /** Create a string input.
   *
   * @param input the string to read
   * @param name the "filename" to use when reporting errors
   */
  static AntlrInputStream* newStringInputStream(const std::string& input, const std::string& name)
    throw (InputStreamException);
};

class Parser;

/**
 * An input to be parsed. The static factory methods in this class (e.g.,
 * <code>newFileInput</code>, <code>newStringInput</code>) create a parser
 * for the given input language and attach it to an input source of the
 * appropriate type.
 */
class AntlrInput : public Input{
  /** The token lookahead used to lex and parse the input. This should usually be equal to
   * <code>K</code> for an LL(k) grammar. */
  unsigned int d_lookahead;

  /** The ANTLR3 lexer associated with this input. This will be <code>NULL</code> initially. It
   *  must be set by a call to <code>setLexer</code>, preferably in the subclass constructor. */
  pANTLR3_LEXER d_lexer;

  /** The ANTLR3 parser associated with this input. This will be <code>NULL</code> initially. It
   *  must be set by a call to <code>setParser</code>, preferably in the subclass constructor.
   *  The <code>super</code> field of <code>d_parser</code> will be set to <code>this</code> and
   *  <code>reportError</code> will be set to <code>Input::reportError</code>. */
  pANTLR3_PARSER d_parser;

  /** The ANTLR3 input stream associated with this input. */
  pANTLR3_INPUT_STREAM d_antlr3InputStream;

  /** The ANTLR3 token stream associated with this input. We only need this so we can free it on exit.
   *  This is set by <code>setLexer</code>.
   *  NOTE: We assume that we <em>can</em> free it on exit. No sharing! */
  pANTLR3_COMMON_TOKEN_STREAM d_tokenStream;

  /** Turns an ANTLR3 exception into a message for the user and calls <code>parseError</code>. */
  static void reportError(pANTLR3_BASE_RECOGNIZER recognizer);

  /** Builds a message for a lexer error and calls <code>parseError</code>. */
  static void lexerError(pANTLR3_BASE_RECOGNIZER recognizer);

  /** Returns the next available lexer token from the current input stream. */
  static pANTLR3_COMMON_TOKEN
  nextTokenStr (pANTLR3_TOKEN_SOURCE toksource);

  /* Since we own d_tokenStream and it needs to be freed, we need to prevent
   * copy construction and assignment.
   */
  AntlrInput(const AntlrInput& input);
  AntlrInput& operator=(const AntlrInput& input);

public:

  /** Destructor. Frees the token stream and closes the input. */
  virtual ~AntlrInput();

  /** Create an input for the given AntlrInputStream. NOTE: the new Input
   * will take ownership of the input stream and delete it at destruction time.
   *
   * @param lang the input language
   * @param inputStream the input stream
   *
   * */
  static AntlrInput* newInput(InputLanguage lang, AntlrInputStream& inputStream);

  /** Retrieve the text associated with a token. */
  static std::string tokenText(pANTLR3_COMMON_TOKEN token);
  /** Retrieve an unsigned from the text of a token */
  static unsigned tokenToUnsigned( pANTLR3_COMMON_TOKEN token );
  /** Retrieve an Integer from the text of a token */
  static Integer tokenToInteger( pANTLR3_COMMON_TOKEN token );
  /** Retrieve a Rational from the text of a token */
  static Rational tokenToRational(pANTLR3_COMMON_TOKEN token);

protected:
  /** Create an input. This input takes ownership of the given input stream,
   * and will delete it at destruction time.
   *
   * @param inputStream the input stream to use
   * @param lookahead the lookahead needed to parse the input (i.e., k for
   * an LL(k) grammar)
   */
  AntlrInput(AntlrInputStream& inputStream, unsigned int lookahead);

  /** Retrieve the token stream for this parser. Must not be called before
   * <code>setLexer()</code>. */
  pANTLR3_COMMON_TOKEN_STREAM getTokenStream();

  /**
   * Throws a <code>ParserException</code> with the given message.
   */
  void parseError(const std::string& msg) throw (ParserException);

  /** Set the ANTLR3 lexer for this input. */
  void setAntlr3Lexer(pANTLR3_LEXER pLexer);

  /** Set the ANTLR3 parser implementation for this input. */
  void setAntlr3Parser(pANTLR3_PARSER pParser);

  /** Set the Parser object for this input. */
  void setParser(Parser *parser);
};

inline std::string AntlrInput::tokenText(pANTLR3_COMMON_TOKEN token) {
  ANTLR3_MARKER start = token->getStartIndex(token);
  ANTLR3_MARKER end = token->getStopIndex(token);
  /* start and end are boundary pointers. The text is a string
   * of (end-start+1) bytes beginning at start. */
  std::string txt( (const char *)start, end-start+1 );
  Debug("parser-extra") << "tokenText: start=" << start << std::endl
                        <<  "end=" << end << std::endl
                        <<  "txt='" << txt << "'" << std::endl;
  return txt;
}

inline unsigned AntlrInput::tokenToUnsigned(pANTLR3_COMMON_TOKEN token) {
  unsigned result;
  std::stringstream ss;
  ss << tokenText(token);
  ss >> result;
  return result;
}

inline Integer AntlrInput::tokenToInteger(pANTLR3_COMMON_TOKEN token) {
  Integer i( tokenText(token) );
  return i;
}

inline Rational AntlrInput::tokenToRational(pANTLR3_COMMON_TOKEN token) {
  Rational r( tokenText(token) );
  return r;
}

}/* CVC4::parser namespace */
}/* CVC4 namespace */

#endif /* __CVC4__PARSER__ANTLR_INPUT_H */
