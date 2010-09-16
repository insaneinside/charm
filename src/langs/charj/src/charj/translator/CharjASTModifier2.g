/**
 *  TODO add a description
 */

tree grammar CharjASTModifier2;

options {
    backtrack = true; 
    memoize = true;
    tokenVocab = Charj;
    ASTLabelType = CharjAST;
    output = AST;
}

@header {
package charj.translator;
}

// Replace default ANTLR generated catch clauses with this action, allowing early failure.
@rulecatch {
    catch (RecognitionException re) {
        reportError(re);
        throw re;
    }
}

@members {
    SymbolTable symtab = null;
    PackageScope currentPackage = null;
    ClassSymbol currentClass = null;
    MethodSymbol currentMethod = null;
    LocalScope currentLocalScope = null;
    Translator translator;

    AstModifier astmod = new AstModifier();

    protected boolean containsModifier(CharjAST modlist, int type)
    {
        if(modlist == null)
            return false;
        CharjAST charjModList = modlist.getChildOfType(CharjParser.CHARJ_MODIFIER_LIST);
        if(charjModList == null)
            return false;
        if(charjModList.getChildOfType(CharjParser.ENTRY) == null)
            return false;
        return true;
    }
}

// Starting point for parsing a Charj file.
charjSource[SymbolTable _symtab] returns [ClassSymbol cs]
    :   ^(CHARJ_SOURCE 
        (packageDeclaration)? 
        (importDeclaration
        | typeDeclaration { $cs = $typeDeclaration.sym; }
        | readonlyDeclaration)*)
    ;

// note: no new scope here--this replaces the default scope
packageDeclaration
    :   ^(PACKAGE IDENT+)
    ;
    
importDeclaration
    :   ^(IMPORT qualifiedIdentifier '.*'?)
    ;

readonlyDeclaration
    :   ^(READONLY localVariableDeclaration)
    ;

typeDeclaration returns [ClassSymbol sym]
    :   ^(TYPE classType IDENT
            (^('extends' parent=type))? (^('implements' type+))?
                classScopeDeclaration*)
    |   ^('interface' IDENT (^('extends' type+))?  interfaceScopeDeclaration*)
    |   ^('enum' IDENT (^('implements' type+))? enumConstant+ classScopeDeclaration*)
    ;

classType
    :   CLASS
    |   chareType
    ;

chareType
    :   CHARE
    |   GROUP
    |   NODEGROUP
    |   MAINCHARE
    |   ^(CHARE_ARRAY ARRAY_DIMENSION)
    ;

enumConstant
    :   ^(IDENT arguments?)
    ;
    
classScopeDeclaration
    :   ^(FUNCTION_METHOD_DECL modifierList? genericTypeParameterList?
            type IDENT formalParameterList domainExpression? b=block)
    |   ^(ENTRY_FUNCTION_DECL modifierList? genericTypeParameterList?
            type IDENT entryFormalParameterList domainExpression? b=block)
    |   ^(PRIMITIVE_VAR_DECLARATION modifierList? simpleType variableDeclaratorList)
            //^(VAR_DECLARATOR_LIST field[$simpleType.type]+))
    |   ^(OBJECT_VAR_DECLARATION modifierList? objectType variableDeclaratorList)
            //^(VAR_DECLARATOR_LIST field[$objectType.type]+))
    |   ^(CONSTRUCTOR_DECL modifierList? genericTypeParameterList? IDENT formalParameterList 
            block)
    |   ^(ENTRY_CONSTRUCTOR_DECL modifierList? genericTypeParameterList? IDENT entryFormalParameterList 
            block)
    ;

field [ClassSymbol type]
    :   ^(VAR_DECLARATOR ^(IDENT domainExpression?) variableInitializer?)
    ;
    
interfaceScopeDeclaration
    :   ^(FUNCTION_METHOD_DECL modifierList? genericTypeParameterList? 
            type IDENT formalParameterList domainExpression?)
        // Interface constant declarations have been switched to variable
        // declarations by Charj.g; the parser has already checked that
        // there's an obligatory initializer.
    |   ^(PRIMITIVE_VAR_DECLARATION modifierList? simpleType variableDeclaratorList)
    |   ^(OBJECT_VAR_DECLARATION modifierList? objectType variableDeclaratorList)
    ;

variableDeclaratorList
    :   ^(VAR_DECLARATOR_LIST variableDeclarator+)
    ;

variableDeclarator
    :   ^(VAR_DECLARATOR variableDeclaratorId variableInitializer?)
    ;
    
variableDeclaratorId
    :   ^(IDENT domainExpression?)
    ;

variableInitializer
    :   arrayInitializer
    |   expression
    ;
    
arrayInitializer
    :   ^(ARRAY_INITIALIZER variableInitializer*)
    ;

templateArg
    : genericTypeArgument
    | literal
    ;

templateArgList
    :   templateArg templateArg*
    ;

templateInstantiation
    :    ^(TEMPLATE_INST templateArgList)
    |    ^(TEMPLATE_INST templateInstantiation)
    ;

genericTypeParameterList
    :   ^(GENERIC_TYPE_PARAM_LIST genericTypeParameter+)
    ;

genericTypeParameter
    :   ^(IDENT bound?)
    ;
        
bound
    :   ^(EXTENDS_BOUND_LIST type+)
    ;

modifierList
    :   ^(MODIFIER_LIST accessModifierList localModifierList charjModifierList otherModifierList)
    ;

modifier
    :   accessModifier
    |   localModifier
    |   charjModifier
    |   otherModifier
    ;

localModifierList
    :   ^(LOCAL_MODIFIER_LIST localModifier*)
    ;

accessModifierList
    :   ^(ACCESS_MODIFIER_LIST accessModifier*)
    ;

charjModifierList
    :   ^(CHARJ_MODIFIER_LIST charjModifier*)
    ;

otherModifierList
    :   ^(OTHER_MODIFIER_LIST otherModifier*)
    ;
    
localModifier
    :   FINAL
    |   STATIC
    |   VOLATILE
    ;

accessModifier
    :   PUBLIC
    |   PROTECTED
    |   PRIVATE
    ;

charjModifier
    :   ENTRY
    |   SDAGENTRY
    |   TRACED
    ;

otherModifier
    :   ABSTRACT
    |   NATIVE
    ;

entryArgType
    :   simpleType
    |   entryArgObjectType
    |   VOID
    ;

type
    :   simpleType
    |   objectType
    |   VOID
    ;

simpleType returns [ClassSymbol type]
    :   ^(SIMPLE_TYPE primitiveType domainExpression?)
    ;
    
objectType returns [ClassSymbol type]
    :   ^(OBJECT_TYPE qualifiedTypeIdent domainExpression?)
    |   ^(REFERENCE_TYPE qualifiedTypeIdent domainExpression?)
    |   ^(PROXY_TYPE qualifiedTypeIdent domainExpression?)
    |   ^(POINTER_TYPE qualifiedTypeIdent domainExpression?)
    ;

entryArgObjectType returns [ClassSymbol type]
    :   ^(OBJECT_TYPE qualifiedTypeIdent domainExpression?)
    |   ^(REFERENCE_TYPE qualifiedTypeIdent domainExpression?)
    |   ^(PROXY_TYPE qualifiedTypeIdent domainExpression?)
    |   ^(POINTER_TYPE qualifiedTypeIdent domainExpression?)
    ;

qualifiedTypeIdent returns [ClassSymbol type]
    :   ^(QUALIFIED_TYPE_IDENT typeIdent+) 
    ;

typeIdent returns [String name]
    :   ^(IDENT templateInstantiation?)
    ;

primitiveType
    :   BOOLEAN
    |   CHAR
    |   BYTE
    |   SHORT
    |   INT
    |   LONG
    |   FLOAT
    |   DOUBLE
    ;

genericTypeArgumentList
    :   ^(GENERIC_TYPE_ARG_LIST genericTypeArgument+)
    ;
    
genericTypeArgument
    :   type
    |   '?'
    ;

formalParameterList
    :   ^(FORMAL_PARAM_LIST formalParameterStandardDecl* formalParameterVarargDecl?) 
    ;

formalParameterStandardDecl
    :   ^(FORMAL_PARAM_STD_DECL localModifierList? type variableDeclaratorId)
    ;

entryFormalParameterList
    :   ^(FORMAL_PARAM_LIST entryFormalParameterStandardDecl* formalParameterVarargDecl?) 
    ;

entryFormalParameterStandardDecl
    :   ^(FORMAL_PARAM_STD_DECL localModifierList? entryArgType variableDeclaratorId)
    ;
    
formalParameterVarargDecl
    :   ^(FORMAL_PARAM_VARARG_DECL localModifierList? type variableDeclaratorId)
    ;
    
// FIXME: is this rule right? Verify that this is ok, I expected something like:
// IDENT (^(DOT qualifiedIdentifier IDENT))*
qualifiedIdentifier
    :   IDENT
    |   ^(DOT qualifiedIdentifier IDENT)
    ;
    
block
    :   ^(BLOCK (blockStatement)*)
    ;
    
blockStatement
    :   localVariableDeclaration
    |   statement
    ;
    
localVariableDeclaration
    :   ^(PRIMITIVE_VAR_DECLARATION localModifierList? simpleType variableDeclaratorList)
    |   ^(OBJECT_VAR_DECLARATION localModifierList? objectType variableDeclaratorList)
    ;

statement
    : nonBlockStatement
    | sdagStatement
    | block
    ;

sdagStatement
    :   ^(OVERLAP block)
    |   ^(WHEN (IDENT expression? formalParameterList)* block)
    ;

nonBlockStatement
    :   ^(ASSERT expression expression?)
    |   ^(IF parenthesizedExpression block block?)
    |   ^(FOR forInit? FOR_EXPR expression? FOR_UPDATE expression* block)
    |   ^(FOR_EACH localModifierList? type IDENT expression block) 
    |   ^(WHILE parenthesizedExpression block)
    |   ^(DO block parenthesizedExpression)
    |   ^(SWITCH parenthesizedExpression switchCaseLabel*)
    |   ^(RETURN expression?)
    |   ^(THROW expression)
    |   ^(BREAK IDENT?) 
    |   ^(CONTINUE IDENT?) 
    |   ^(LABELED_STATEMENT IDENT statement)
    |   expression
    |   ^('delete' expression)
    |   ^(EMBED STRING_LITERAL EMBED_BLOCK)
    |   ';' // Empty statement.
    |   ^(PRINT expression*)
    |   ^(PRINTLN expression*)
    |   ^(EXIT expression?)
    |   EXITALL
    ;
        
switchCaseLabel
    :   ^(CASE expression blockStatement*)
    |   ^(DEFAULT blockStatement*)
    ;
    
forInit
    :   localVariableDeclaration 
    |   expression+
    ;
    
// EXPRESSIONS

parenthesizedExpression
    :   ^(PAREN_EXPR expression)
    ;
    
expression
    :   ^(EXPR expr)
    ;

expr
    :   ^(ASSIGNMENT expr expr)
    |   ^(PLUS_EQUALS expr expr)
    |   ^(MINUS_EQUALS expr expr)
    |   ^(TIMES_EQUALS expr expr)
    |   ^(DIVIDE_EQUALS expr expr)
    |   ^(AND_EQUALS expr expr)
    |   ^(OR_EQUALS expr expr)
    |   ^(POWER_EQUALS expr expr)
    |   ^(MOD_EQUALS expr expr)
    |   ^('>>>=' expr expr)
    |   ^('>>=' expr expr)
    |   ^('<<=' expr expr)
    |   ^('?' expr expr expr)
    |   ^(OR expr expr)
    |   ^(AND expr expr)
    |   ^(BITWISE_OR expr expr)
    |   ^(POWER expr expr)
    |   ^(BITWISE_AND expr expr)
    |   ^(EQUALS expr expr)
    |   ^(NOT_EQUALS expr expr)
    |   ^(INSTANCEOF expr type)
    |   ^(LTE expr expr)
    |   ^(GTE expr expr)
    |   ^('>>>' expr expr)
    |   ^('>>' expr expr)
    |   ^(GT expr expr)
    |   ^('<<' expr expr)
    |   ^(LT expr expr)
    |   ^(PLUS expr expr)
    |   ^(MINUS expr expr)
    |   ^(TIMES expr expr)
    |   ^(DIVIDE expr expr)
    |   ^(MOD expr expr)
    |   ^(UNARY_PLUS expr)
    |   ^(UNARY_MINUS expr)
    |   ^(PRE_INC expr)
    |   ^(PRE_DEC expr)
    |   ^(POST_INC expr)
    |   ^(POST_DEC expr)
    |   ^(TILDE expr)
    |   ^(NOT expr)
    |   ^(CAST_EXPR type expr)
    |   primaryExpression
    ;
    
primaryExpression
    :   ^(DOT primaryExpression
                (   IDENT
                |   THIS
                |   SUPER
                )
        )
    |   ^(ARROW primaryExpression
                (   IDENT
                |   THIS
                |   SUPER
                )
        )
    |   parenthesizedExpression
    |   IDENT
    |   CHELPER
    |   ^(METHOD_CALL primaryExpression genericTypeArgumentList? arguments)
    |   ^(ENTRY_METHOD_CALL primaryExpression genericTypeArgumentList? entryArguments)
    |   explicitConstructorCall
    |   ^(ARRAY_ELEMENT_ACCESS primaryExpression expression)
    |   ^(ARRAY_ELEMENT_ACCESS primaryExpression domainExpression)
    |   literal
    |   newExpression
    |   THIS
    |   arrayTypeDeclarator
    |   SUPER
    |   GETNUMPES
    |   GETNUMNODES
    |   GETMYPE
    |   GETMYNODE
    |   GETMYRANK
	|	THISINDEX
	|	THISPROXY
    |   domainExpression
    ;
    
explicitConstructorCall
    :   ^(THIS_CONSTRUCTOR_CALL genericTypeArgumentList? arguments)
    |   ^(SUPER_CONSTRUCTOR_CALL primaryExpression? genericTypeArgumentList? arguments)
    ;

arrayTypeDeclarator
    :   ^(ARRAY_DECLARATOR (arrayTypeDeclarator | qualifiedIdentifier | primitiveType))
    ;

newExpression
    :   ^(NEW_EXPRESSION arguments? domainExpression)
    |   ^(NEW type arguments)
    ;

arguments
    :   ^(ARGUMENT_LIST expression*)
    ;

entryArguments
    :   ^(ARGUMENT_LIST entryArgExpr*)
    ;

entryArgExpr
    :   ^(EXPR entryExpr)
    ;

entryExpr
    :   ^(ASSIGNMENT expr expr)
    |   ^(PLUS_EQUALS expr expr)
    |   ^(MINUS_EQUALS expr expr)
    |   ^(TIMES_EQUALS expr expr)
    |   ^(DIVIDE_EQUALS expr expr)
    |   ^(AND_EQUALS expr expr)
    |   ^(OR_EQUALS expr expr)
    |   ^(POWER_EQUALS expr expr)
    |   ^(MOD_EQUALS expr expr)
    |   ^('>>>=' expr expr)
    |   ^('>>=' expr expr)
    |   ^('<<=' expr expr)
    |   ^('?' expr expr expr)
    |   ^(OR expr expr)
    |   ^(AND expr expr)
    |   ^(BITWISE_OR expr expr)
    |   ^(POWER expr expr)
    |   ^(BITWISE_AND expr expr)
    |   ^(EQUALS expr expr)
    |   ^(NOT_EQUALS expr expr)
    |   ^(INSTANCEOF expr type)
    |   ^(LTE expr expr)
    |   ^(GTE expr expr)
    |   ^('>>>' expr expr)
    |   ^('>>' expr expr)
    |   ^(GT expr expr)
    |   ^('<<' expr expr)
    |   ^(LT expr expr)
    |   ^(PLUS expr expr)
    |   ^(MINUS expr expr)
    |   ^(TIMES expr expr)
    |   ^(DIVIDE expr expr)
    |   ^(MOD expr expr)
    |   ^(UNARY_PLUS expr)
    |   ^(UNARY_MINUS expr)
    |   ^(PRE_INC expr)
    |   ^(PRE_DEC expr)
    |   ^(POST_INC expr)
    |   ^(POST_DEC expr)
    |   ^(TILDE expr)
    |   ^(NOT expr)
    |   ^(CAST_EXPR type expr)
    |   entryPrimaryExpression
    ;
    
entryPrimaryExpression
    :   ^(DOT primaryExpression
                (   IDENT
                |   THIS
                |   SUPER
                )
        )
    |   ^(ARROW primaryExpression
                (   IDENT
                |   THIS
                |   SUPER
                )
        )
    |   parenthesizedExpression
    |   IDENT
        {
            //System.out.println("Derefing ID with type " + $IDENT.symbolType.getClass().getName() +
            //    ":\n" + $IDENT.symbolType + "\nand def info " + $IDENT.def.getClass().getName() + ":\n" + $IDENT.def);
            if ($IDENT.symbolType instanceof ClassSymbol) {
                if (!((ClassSymbol)$IDENT.symbolType).isPrimitive) {
                    astmod.makePointerDereference($IDENT);
                }
            }
        }
    |   ^(METHOD_CALL primaryExpression genericTypeArgumentList? arguments)
    |   ^(ENTRY_METHOD_CALL primaryExpression genericTypeArgumentList? entryArguments)
    |   explicitConstructorCall
    |   ^(ARRAY_ELEMENT_ACCESS primaryExpression expression)
    |   literal
    |   newExpression
        ->  ^(POINTER_DEREFERENCE newExpression)
    |   THIS
        ->  ^(POINTER_DEREFERENCE THIS)
    |   arrayTypeDeclarator
    |   SUPER
    |   GETNUMPES
    |   GETNUMNODES
    |   GETMYPE
    |   GETMYNODE
    |   GETMYRANK
    |   domainExpression
    ;

literal 
    :   HEX_LITERAL
    |   OCTAL_LITERAL
    |   DECIMAL_LITERAL
    |   FLOATING_POINT_LITERAL
    |   CHARACTER_LITERAL
    |   STRING_LITERAL          
    |   TRUE
    |   FALSE
    |   NULL 
    ;

rangeItem
    :   DECIMAL_LITERAL
    |   IDENT
    ;

rangeExpression
    :   ^(RANGE_EXPRESSION rangeItem)
    |   ^(RANGE_EXPRESSION rangeItem rangeItem)
    |   ^(RANGE_EXPRESSION rangeItem rangeItem rangeItem)
    ;

rangeList
    :   rangeExpression+
    ;

domainExpression
    :   ^(DOMAIN_EXPRESSION rangeList)
    ;
