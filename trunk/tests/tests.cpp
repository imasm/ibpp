///////////////////////////////////////////////////////////////////////////////
//
//	File    : $Id$
//	Subject : IBPP, TEST1 program
//
///////////////////////////////////////////////////////////////////////////////
//
//	The contents of this file are subject to the IBPP License Version 1.0
//	(the "License"); you may not use this file except in compliance with the
//	License.  You may obtain a copy of the License at http://www.ibpp.org or
//	in the 'license.txt' file which must have been distributed along with this
//	file.
//
//	This software, distributed under the License, is distributed on an "AS IS"
//	basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the
//	License for the specific language governing rights and limitations
//	under the License.
//
//	Contributor(s):
//
///////////////////////////////////////////////////////////////////////////////
//
//	COMMENTS
//	* Tabulations should be set every four characters when editing this file.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
#pragma warning(disable: 4786 4996)
#ifndef _DEBUG
#pragma warning(disable: 4702)
#endif
#endif

#ifdef IBPP_WINDOWS
#include <windows.h>
#endif

#include "../core/ibpp.h"

#ifdef IBPP_UNIX
#include <unistd.h>
#define DeleteFile(x) unlink(x)
#define Sleep(x) usleep(1000 * x)
#endif

#ifdef HAS_HDRSTOP
#pragma hdrstop
#endif

#include <iostream>
#include <stdio.h>
#include <typeinfo>

//	The following database name (DbName) will be used during this test.
//	If existing, that DB will be deleted (assuming not used).
//	It will then be dynamically created, and various tests will run
//	while it is being populated and exercised.
//
//	The DB should grow to around 25 MB while being exercised.

#ifdef IBPP_UNIX
	char* DbName = "/usr/tmp/test.fdb";
	char* BkName = "/usr/tmp/test.fbk";
	std::string ServerName = "localhost";
#else
	char* DbName = "C:/test.fdb";	// FDB extension (GDB is hacked by Windows Me/XP "System Restore")
	char* BkName = "C:/test.fbk";
	std::string ServerName = "localhost";
#endif

//	The tests use by default the well-known default of SYSDBA/masterkey
//	credentials. Do not forget to update them if required.

std::string UserName = "SYSDBA";
std::string Password = "masterkey";

class Test
{
	// Class 'Test' drives all the tests of this module.

	bool _Success;				// if true after all tests, everything succeeded
	int _WriteMode;				// 0 == default, 1 == speed, 2 == safety

	void Test1(void);
	void Test2(void); 
	void Test3(void);
	void Test4(void);
	void Test5(void);
	void Test6(void);
	void Test7(void);
	void Test8(void);

public:
	void RunTests(void);
	int PrintResult(void);
	Test(int argc, char* argv[]);
	~Test();
};

int Test::PrintResult(void)
{
	if (_Success) printf(_("\n*** SUCCESS ***\nAll tests were 100%% successfull.\n"));
	else printf(_("\n*** FAILED ***\nSome tests failed, as indicated above.\n"));
	return _Success ? 0 : 1;
}

void Test::RunTests(void)
{
	int NextTest = 1;

	printf(_("\nIBPP Test Suite (library version %d.%d.%d.%d)\n\n"),
		(IBPP::Version & 0xFF000000) >> 24,
		(IBPP::Version & 0x00FF0000) >> 16,
		(IBPP::Version & 0x0000FF00) >> 8,
		(IBPP::Version & 0x000000FF));

	for (;;)
	{
		try
		{
			switch (NextTest++)
			{
				case 1 :	Test1(); break;
				case 2 :	Test2(); break;
				case 3 :	Test3(); break;
				case 4 :	Test4(); break;
				case 5 :	Test5(); break;
				case 6 :	Test6(); break;
				case 7 :	Test7(); break;
				case 8 :	Test8(); break;
				default :	return;		// All tests have been run
			}
		}
		// You don't need to catch all these.
		// This is done only as a detailed sample case.
		// Often catching IBPP::Exception& is enough just to report the error string.
		// If you need access to the SQLCode and EngineCode obtained when the error
		// comes from the engine, catch(SQLException&) in addition or use RTTI.
		catch(IBPP::Exception& e)
		{
			_Success = false;
			std::cout << e.what() << "\n";
			IBPP::SQLException* pe = dynamic_cast<IBPP::SQLException*>(&e);
			if (pe == 0) std::cout<< _("Not an engine error\n");
			else std::cout<< "Engine Code : "<< pe->EngineCode()<< "\n";
		}
		catch(std::exception& e)
		{
			_Success = false;
			std::cout << e.what() << "\n";
			printf(_("Test %d --- FAILED\n")
				_("A std::exception '%s' occured !\n\n"),
					NextTest-1, typeid(e).name());
		}
		catch(...)
		{
			_Success = false;
			printf(_("Test %d --- FAILED\n")
				_("A system exception occured !\n\n"), NextTest-1);
		}
	}
}

void Test::Test1(void)
{
	printf(_("Test 1 --- Checking Date/Time (no DB involved)\n"));

	IBPP::Date dt;
	IBPP::Date dt1;
	IBPP::Date dt2;
	IBPP::Timestamp tm1;
	int asint;
	int year, month, day;
	int hour, min, sec;

	tm1.Now();
	tm1.GetDate(year, month, day);
	tm1.GetTime(hour, min, sec);
	printf(_("           Now : Y=%d M=%d D=%d, %d:%d:%d\n"),
		year, month, day, hour, min, sec);

	dt.SetDate(1965, 4, 5);		// Who's birthday is that ? :)
	tm1 = dt;
	tm1.Add(1);
	tm1.Add(-1);
	dt = tm1;
	asint = dt.GetDate();
	if (asint != 23836)
	{
		_Success = false;
		printf(_("Failed '5 April 1965' date as int test.\n")
			_("Returned %d while 23836 was expected.\n"), asint);
		return;
	}

	dt.SetDate(1900, 1, 1);
	asint = dt.GetDate();
	if (asint != 1)
	{
		_Success = false;
		printf(_("Failed '1 January 1900' date as int test.\n")
			_("Returned %d while 1 was expected.\n"), asint);
		return;
	}

	dt.SetDate(1, 1, 1);	// 1 january 0001
	asint = dt.GetDate();
	if (asint != -693594)
	{
		_Success = false;
		printf(_("Failed '1 January 0001' date as int test.\n")
			_("Returned %d while -693594 was expected.\n"), asint);
		return;
	}
	dt.SetDate(-693594);
	dt.GetDate(year, month, day);
	if (year != 1 || month != 1 || day != 1)
	{
		_Success = false;
		printf(_("Failed -693594 as date test.\n")
			_("Returned Y:%d M:%d D:%d while 1 January 1 was expected.\n"),
			 year, month, day);
		return;
	}

	dt.SetDate(9999, 12, 31);	// 31 december 9999
	asint = dt.GetDate();
	if (asint != 2958464)
	{
		_Success = false;
		printf(_("Failed '31 December 9999' date as int test.\n")
			_("Returned %d while 2958464 was expected.\n"), asint);
		return;
	}

	dt.SetDate(2001, 2, 12);
	asint = dt.GetDate();
	if (asint != 36933)
	{
		_Success = false;
		printf(_("Failed '12 February 2001' date as int test.\n")
			_("Returned %d while 36933 was expected.\n"), asint);
		return;
	}

	dt.SetDate(asint);
	dt2 = dt;
	dt2.Add(15);
	if (dt2.GetDate() != 36948)
	{
		_Success = false;
		printf(_("Failed Date::Add() test.\n")
			_("Returned %d while 36948 was expected.\n"), dt2.GetDate());
		return;
	}

	int diff = dt2.GetDate() - dt.GetDate();
	if (diff != 15)
	{
		_Success = false;
		printf(_("Failed date arithmetic test.\n")
			_("Returned %d while 15 was expected.\n"), diff);
		return;
	}
}

void Test::Test2(void)
{
	printf(_("Test 2 --- Exercise empty database creation & connection\n"));

	int Major, Minor, PageSize, Pages, Buffers, Sweep;
	bool Sync, Reserve;

	IBPP::Database db1;
	DeleteFile(DbName);
	db1 = IBPP::DatabaseFactory(ServerName, DbName, UserName, Password,
		"", "WIN1252", "PAGE_SIZE 8192 DEFAULT CHARACTER SET WIN1252");
	db1->Create(3);		// 3 is the dialect of the database (could have been 1)

	IBPP::Service svc = IBPP::ServiceFactory(ServerName, UserName, Password);
	svc->Connect();
	svc->SetPageBuffers(DbName, 256);	// Instead of default 2048
	svc->SetSweepInterval(DbName, 5000);	// instead of 20000 by default
	if (_WriteMode == 1) svc->SetSyncWrite(DbName, false);
	else if (_WriteMode == 2) svc->SetSyncWrite(DbName, true);
	svc->SetReadOnly(DbName, false);	// That's the default anyway
	svc->Disconnect();

	db1->Connect();	// A create, does not imply connection

	db1->Info(&Major, &Minor, &PageSize, &Pages, &Buffers, &Sweep, &Sync, &Reserve);
	if (Sync && _WriteMode == 1)
	{
		_Success = false;
		printf(_("The created database has sync writes enabled (safety),\n"
			"while it was expected to be disabled (speed).\n"));
	}

	if (! Sync && _WriteMode == 2)
	{
		_Success = false;
		printf(_("The created database has sync writes disabled (speed),\n"
			"while it was expected to be enabled (safety).\n"));
	}

	if (Sync) printf(_("           Sync Writes is enabled (Safety).\n"
		"           Use 'speed' command-line argument to test the other mode.\n"));
    else printf(_("           Sync Writes is disabled (Speed).\n"
		"           Use 'safety' command-line argument to test the other mode.\n"));
 
	/**/
	printf("           ODS Major %d\n", Major);
	printf("           ODS Minor %d\n", Minor);
	printf("           Page Size %d\n", PageSize);
	printf("           Pages     %d\n", Pages);
	printf("           Buffers   %d\n", Buffers);
	printf("           Sweep     %d\n", Sweep);
	printf("           Reserve   %s\n", Reserve ? _("true") : _("false"));
	/**/

    db1->Disconnect();
}

void Test::Test3(void)
{
	printf(_("Test 3 --- Exercise basic DDL operations and IBPP::Exceptions\n"));

	IBPP::Database db1;
	db1 = IBPP::DatabaseFactory(ServerName, DbName, UserName, Password);
	db1->Connect();

	// The following transaction configuration values are the defaults and
	// those parameters could have as well be omitted to simplify writing.
	IBPP::Transaction tr1 = IBPP::TransactionFactory(db1,
							IBPP::amWrite, IBPP::ilConcurrency, IBPP::lrWait);
	tr1->Start();

	IBPP::Statement st1 = IBPP::StatementFactory(db1, tr1);

	st1->ExecuteImmediate(	"CREATE TABLE TEST("
							"N2 NUMERIC(9,2), "
							"N6 NUMERIC(15,2), "
							"N5 NUMERIC(9,5), "
							"A1 NUMERIC(9,2) [8], "
							"A2 VARCHAR(30) [0:3, 1:4], "
				 //			"A3 INTEGER [0:1199], "
							"DA TIMESTAMP [0:1], "
							"D DATE, "
							"T TIME, "
							"TS TIMESTAMP, "
							"B BLOB SUB_TYPE 1, "
							"BB BLOB SUB_TYPE 0, "
							"TF CHAR(1), "
							"ID INTEGER, "
							"TX CHAR(30), "
							"VX VARCHAR(30), "
							"TB CHAR(40) CHARACTER SET OCTETS, "
							"VB VARCHAR(40) CHARACTER SET OCTETS)");
	tr1->CommitRetain();
	st1->ExecuteImmediate(	"CREATE VIEW PRODUCT(X, Y) AS "
							"SELECT T.N2, S.N2 FROM TEST T, TEST S"
							);
	tr1->CommitRetain();
	try
	{
		#if defined(IBPP_WINDOWS) && defined(_DEBUG)
			OutputDebugString(_("An exception will now get logged in the debugger: this is expected.\n"));
		#endif
		st1->ExecuteImmediate(	"CREATE SYNTAX ERROR(X, Y) AS "
								"SELECT ERRONEOUS FROM MUSTFAIL M" );
	}
	catch(IBPP::SQLException& e)
	{
		//~ std::cout<< e.what();

		if (e.EngineCode() != 335544569)
		{
			_Success = false;
			printf(_("The error code returned by the engine during a\n"
				"voluntary statement syntax error is unexpected.\n"));
		}
	}

	// Intentionally do not Commit or Rollback the transaction, nor even Disconnect
	// The auto-release mechanisms will have to Rollback and terminate everything cleanly.
}

void Test::Test4(void)
{
	printf(_("Test 4 --- Populate database and exercise Blobs and Arrays (100 rows)\n"));

	IBPP::Database db1;
	db1 = IBPP::DatabaseFactory(ServerName, DbName, UserName, Password);
	db1->Connect();

	// The following transaction configuration values are the defaults and
	// those parameters could have as well be omitted to simplify writing.
	IBPP::Transaction tr1 = IBPP::TransactionFactory(db1,
							IBPP::amWrite, IBPP::ilConcurrency, IBPP::lrWait);
	tr1->Start();

	IBPP::Statement st1 = IBPP::StatementFactory(db1, tr1);

	IBPP::Blob b1 = IBPP::BlobFactory(db1, tr1);
	IBPP::Blob bb = IBPP::BlobFactory(db1, tr1);
	IBPP::Array ar1 = IBPP::ArrayFactory(db1, tr1);
	IBPP::Array ar2 = IBPP::ArrayFactory(db1, tr1);
	//IBPP::Array ar4 = IBPP::ArrayFactory(db1, tr1);
	IBPP::Array da = IBPP::ArrayFactory(db1, tr1);

	// Checking the new date and time support
	st1->ExecuteImmediate("insert into test(D, T, TS) "
		"values('2004-02-29', '10:11:12.1314', '1858-11-18 10:11:12.1314')");

	st1->Execute("select D, T, TS from test");
	st1->Fetch();
	IBPP::Date dt;
	IBPP::Time tm;
	IBPP::Timestamp ts;
	st1->Get(1, dt);
	st1->Get(2, tm);
	st1->Get(3, ts);
	
	int y, m, d, h, min, s, t;
	dt.GetDate(y, m, d);
	if (y != 2004 || m != 2 || d != 29)
	{
		_Success = false;
		printf("Date storage anomaly : %d / %d / %d\n", y, m, d);
		return;
	}
	
	tm.GetTime(h, min, s, t);
	if (h != 10 || min != 11 || s != 12 || t != 1314)
	{
		_Success = false;
		printf("Time storage anomaly : %d : %d : %d : %d\n", h, min, s, t);
		return;
	}

	ts.GetDate(y, m, d);
	ts.GetTime(h, min, s, t);
	if (y != 1858 || m != 11 || d != 18 ||
		h != 10 || min != 11 || s != 12 || t != 1314)
	{
		_Success = false;
		printf("Timestamp storage anomaly : %d / %d / %d, %d : %d : %d : %d\n",
			y, m, d, h, min, s, t);
		return;
	}
	st1->ExecuteImmediate("delete from test");

	st1->Prepare("insert into test(N2,N6,N5,D,B,BB,TF,ID,A1,A2,DA,TX,VX,TB,VB) "
					"values(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
	if (st1->Parameters() != 15)
	{
		_Success = false;
		printf(_("Statement::Parameters() failed to return correct count of parameters."));
	}
	else
	{
		if (st1->ParameterType(13) != IBPP::sdString ||
			st1->ParameterSubtype(5) != 1 ||
			st1->ParameterSize(13) != 30)
		{
			_Success = false;
			printf(_("Statement::ParameterType(), Subtype() or Size() failed to return correct value."));
		}
	}
	st1->Set(1, 12.3456);
	st1->Set(2, 12.3456);
	st1->Set(3, 987.65432);
	IBPP::Date dt1;
	dt1.SetDate(1850, 2, 12);
	IBPP::Date dt2 = dt1;
	st1->Set(4, dt2);

	double a1[8] = {11.123, 12.126, 13.0, 14.0, 15.0, 16.0, 17.0, 18.0};
	char a2[4][4][31] = {
							{"UN", "DEUX", "TROIS", "QUATRE"},
							{"CINQ", "SIX", "SEPT", "HUIT"},
							{"NEUF", "DIX", "ONZE", "DOUZE"},
							{"TREIZE", "QUATORZE", "QUINZE", "SEIZE"} };
	IBPP::Timestamp da1[2];
	da1[0].SetDate(2002, 8, 14);
	da1[1].SetDate(2003, 2, 28);
	da1[0].SetTime(15, 16, 17);
	da1[1].SetTime(18, 19, 20);

	ar1->Describe("TEST", "A1");
	ar2->Describe("TEST", "A2");
	da->Describe("TEST", "DA");

	int i;
	/*
	printf("A1 type  = %d\n", ar1->ElementType());
	printf("A1 size  = %d\n", ar1->ElementSize());
	printf("A1 scale = %d\n", ar1->ElementScale());
	printf("A1 dim   = %d\n", ar1->Dimensions());

	for (i = 0; i < ar1->Dimensions(); i++)
	{
		int low, high;
		ar1->Bounds(i, &low, &high);
		printf("Dim %i = [%d:%d]\n", i, low, high);
	}
	*/

	/*
	// Huge array testing
	int* pa4 = new int[1200];
	for (i = 0; i < 1200; i++) pa4[i] = i;
	ar4->Describe("TEST", "A3");
	*/

	std::string stdstring = "STD::STRING";

	int total = 0;
	int somebytes[10] = { 1, 2, 3, 0, 5, 6, 7, 8, 0, 10 };

	FILE* file = fopen("blob.txt", "w");
	for (i = 0; i < 1000; i++)
		fputs(_("Dummy blob data for running some blob input/output tests.\n"), file);
	fclose(file);
	for (i = 0; i < 100; i++)
	{
		// Writing a blob, using the low-level interface
		char buffer[10240];
		int len;
		b1->Create();
		FILE* file = fopen("blob.txt", "r");
		while ((len = (int)fread(buffer, 1, 10240, file)) == 10240)
		{
			b1->Write(buffer, 10240);
			total += 10240;
		}
		b1->Write(buffer, len);
		total += len;
		b1->Close();
		fclose(file);
		st1->Set(5, b1);

		// Writing a blob, using the std:string interface
		std::string bbs;
		file = fopen("blob.txt", "r");
		bbs.resize(40000);
		fread(const_cast<char*>(bbs.data()), 1, 40000, file);
		fclose(file);
		//bb->Save(bbs);
		//st1->Set(6, bb);

		// Third, direct way of writing a std::string to a blob
		st1->Set(6, bbs);

		st1->Set(7, (i%2) != 0);
#ifdef __DMC__
		st1->Set(8, (int32_t)i);
#else
		st1->Set(8, i);
#endif

		ar1->WriteFrom(IBPP::adDouble, a1, sizeof(a1)/sizeof(double));
		st1->Set(9, ar1);
		ar2->WriteFrom(IBPP::adString, a2, sizeof(a2)/sizeof(char[31]));
		st1->Set(10, ar2);
		da->WriteFrom(IBPP::adTimestamp, da1, sizeof(da1)/sizeof(IBPP::Timestamp));
		st1->Set(11, da);

		st1->Set(12, "C-STRING");
		st1->Set(13, stdstring);

		st1->Set(14, (char*)somebytes, 40);
		st1->Set(15, (char*)somebytes, 32);

		/*
		ar4->WriteFrom(IBPP::adInt, pa4, 1200);
		st1->Set(16, ar4);
		*/

		st1->Execute();
	}
	total /= 100;
	DeleteFile("blob.txt");

	tr1->CommitRetain();

	st1->Prepare("select B, BB, A2 from test where ID = 1");

	std::string plan;
	st1->Plan(plan);
	printf("           Plan: ");
	printf(plan.c_str());
	printf("\n");
	
	st1->Execute();
	IBPP::Row row;
	st1->Fetch(row);
	IBPP::Row row2 = row->Clone();	// row2 is a real copy not a second pointer to same data

	IBPP::Blob b2 = IBPP::BlobFactory(db1, tr1);
	IBPP::Blob bb2 = IBPP::BlobFactory(db1, tr1);
	IBPP::Array ar3 = IBPP::ArrayFactory(db1, tr1);
	ar3->Describe("TEST", "A2");
	char buffer[1024];
	int size, largest, segments;

	row2->Get(1, b2);
	b2->Open();
	b2->Info(&size, &largest, &segments);
	if (size != total)
	{
		_Success = false;
		printf(_("Blob size incorrect on read back from the database.\n"));
		printf(_("Read %d bytes, expected %d\n"), size, total);
	}
	b2->Read(buffer, 1024);
	fflush(stdout);
	b2->Close();

	std::string bbs;
	//row2->Get(2, bb2);
	//bb2->Load(bbs);
	row2->Get(2, bbs);
	//printf("Size = %d\n", bbs.size());

	row2->Get(3, ar3);
	char a3[2][2][31] = {	{"", ""},
							{"", ""} };
	// The stored array is 4 x 4 (0:3 by 1:4).
	// We only want to retrieve the central squarre of 2 x 2 (1:2 by 2:3)
	ar3->SetBounds(0, 1, 2);
	ar3->SetBounds(1, 2, 3);
	ar3->ReadTo(IBPP::adString, a3, sizeof(a3)/sizeof(char[31]));
	/*
	printf("%s %s\n", a3[0][0], a3[0][1]);
	printf("%s %s\n", a3[1][0], a3[1][1]);
	*/
	if (strcmp(a3[0][0], "SIX") != 0 ||
		strcmp(a3[0][1], "SEPT") != 0 ||
		strcmp(a3[1][0], "DIX") != 0 ||
		strcmp(a3[1][1], "ONZE") != 0)
	{
		_Success = false;
		printf(_("Array testing didn't returned the expected values.\n"));
	}

	/*
	ar3->Describe("TEST", "A3");
	st1->Get(4, ar3);
	for (i = 0; i < 1200; i++) pa4[i] = -1;
	ar3->ReadTo(IBPP::adInt, pa4, 1200);
	for (i = 0; i < 1200; i++)
	{
		if (pa4[i] != i)
		{
			_Success = false;
			printf(_("Big array testing didn't returned the expected values.\n"));
			printf(_("Got %d for element %d instead of %d.\n"), pa4[i], i, i);
			break;
		}
	}

	delete [] pa4;
	*/

	st1->Close();	// unneeded : just to test
	tr1->Commit();
	db1->Disconnect();
}

void Test::Test5(void)
{
	printf(_("Test 5 --- Cocktail of DML statements (100 rows)\n"));

	IBPP::Database db1;
	db1 = IBPP::DatabaseFactory(ServerName, DbName, UserName, Password);
	db1->Connect();

	// The following transaction configuration values are the defaults and
	// those parameters could have as well be omitted to simplify writing.
	IBPP::Transaction tr1 = IBPP::TransactionFactory(db1, IBPP::amWrite,
		IBPP::ilConcurrency, IBPP::lrWait, IBPP::tfNoAutoUndo);
	tr1->AddReservation(db1.intf(), "TEST", IBPP::trProtectedWrite);
	tr1->Start();

	IBPP::Statement st1 = IBPP::StatementFactory(db1, tr1);

	IBPP::Blob b1 = IBPP::BlobFactory(db1, tr1);

	st1->Prepare("select ID, rdb$db_key, N2, D, TF, TX, VX from TEST FOR UPDATE");
	st1->CursorExecute("MYCURSOR");
	/*
	for (int ii = 1; ii <= st1->Columns(); ii++)
	{
		printf("Column %s, Alias %s, Table %s, Type %d, Size %d, Scale %d\n",
			st1->ColumnName(ii), st1->ColumnAlias(ii), st1->ColumnTable(ii),
			(int)st1->ColumnType(ii), st1->ColumnSize(ii), st1->ColumnScale(ii));
	}
	*/

	IBPP::Statement st2 = IBPP::StatementFactory(db1, tr1,
		"UPDATE TEST set n2 = ?, TF = ? where current of MYCURSOR");
	int incr = 0;
	while (st1->Fetch())
	{
		IBPP::Date d2;
		IBPP::Date d3;
		int y, m, d;
		int temp;
		char cstring[31];
		std::string stdstring;

		st1->Get(4, d2);
		temp = d2.GetDate();
		d3 = temp;
		d3.Add(-incr);
		incr++;
		d3.GetDate(y, m, d);
		//printf("%d, %d, %d ",  y, m, d);
		double tmp;
		st1->Get(3, &tmp);
		st2->Set(1, tmp*40.0);
		bool b;
		st1->Get(5, &b);
		bool c;
		st1->Get(5, c);
		if (b != c)
		{
			_Success = false;
			printf(_("Statement::Get(int, bool&) is not working.\n"));
		}
		//printf("%s\n", b ? "true" : "false");
		/*
		printf("%s\n", row->Get("TF"));
		printf("%d\n", row->Get("ID"));
		*/
		st1->Get(6, cstring);
		st1->Get(7, stdstring);

		st2->Set(2, ! b);
		st2->Execute();
	}

	st1->Prepare("select sum(N2),sum(N6) from test");
	st1->Execute();
	while (st1->Fetch())
	{
		double n2, n6;
		st1->Get(1, &n2);
		st1->Get(2, &n6);
		//double n2 = st1->GetDouble(1);
		//double n6 = st1->GetDouble(2);
		//printf("%g, %g\n", n2, n6);
	}

	//printf(_("Select returning no rows...\n"));
	st1->Prepare("select N2 from test where N2 = 456278");
	st1->Execute();
	while (st1->Fetch())
	{
		_Success = false;
		printf(_("Statement::Fetch() not working.\n"
			"Returned row when there is none.\n"));
		//double n2 = st1->GetDouble(1);
		//double n6 = st1->GetDouble(2);
		//printf("%g, %g\n", n2, n6);
	}

	//printf(_("Again, without prepare again...\n"));
	st1->Execute();
	while (st1->Fetch())
	{
		_Success = false;
		printf(_("Statement::Fetch() not working.\n"
			"Returned row when there is none.\n"));
		//double n2 = st1->GetDouble(1);
		//double n6 = st1->GetDouble(2);
		//printf("%g, %g\n", n2, n6);
	}


    //	printf(_("Executing 10 times an Execute, Prepared once...\n"));
	st1->Prepare("select N2, N6 from test");
	for (int i = 0; i < 10; i++)
	{
		//printf(_("Loop %d\n"), i);
		st1->Execute();
		while (st1->Fetch())
		{
			double n2, n6;
 			st1->Get(1, &n2);
			st1->Get(2, &n6);
			//printf("%g, %g\n", n2, n6);
		}
	}

    //	printf(_("Testing IBPP::Row...\n"));
	std::vector<IBPP::Row> rows;
	IBPP::Row r;
	st1->Execute("select N2, N6 from test");
	while (st1->Fetch(r))
		rows.push_back(r);

	for (unsigned i = 0; i < rows.size(); i++)
	{
		double n2, n6;
		rows[i]->Get(1, n2);
		rows[i]->Get(2, n6);
		//printf("%g, %g\n", n2, n6);
	}

	// The rows vector will be deleted when function returns, which
	// will release all individual rows and their storage. Check for leaks !
	
	tr1->Commit();
}

void Test::Test6(void)
{
	printf(_("Test 6 --- Service APIs\n"));

	IBPP::Service svc = IBPP::ServiceFactory(ServerName, UserName, Password);
	svc->Connect();
	
	try
	{
		std::string version;
		svc->GetVersion(version);
		printf("           %s\n", version.c_str());
	}
	catch(IBPP::Exception&)
	{
		printf(_("           Failed getting server version.\n"
			"           Not all InterBase & Firebird servers support this call.\n"));
	}

	printf("           Shutdown...\n");
	svc->Shutdown(DbName, IBPP::dsForce, 10);

	printf("           Validate...\n");
	svc->Repair(DbName, IBPP::RPF(IBPP::rpMendRecords |
							IBPP::rpIgnoreChecksums | IBPP::rpKillShadows));

	printf("           Restart...\n");
	svc->Restart(DbName);

	printf("           Sweep...\n");
	svc->Sweep(DbName);

	printf("           Backup...\n");
	svc->StartBackup(DbName, BkName, IBPP::BRF(IBPP::brNoGarbageCollect |
							IBPP::brVerbose));
	//svc->StartBackup(DbName, BkName, IBPP::brVerbose);
	//svc->StartBackup(DbName, BkName);

	//char* line;
   	//while ((line = (char*)svc->WaitMsg()) != 0) printf("%s\n", line);
	svc->Wait();

	IBPP::Database db1;
	db1 = IBPP::DatabaseFactory(ServerName, DbName, UserName, Password);
	db1->Connect();
	db1->Drop();

	printf("           Restore...\n");
	svc->StartRestore(BkName, DbName, 0, IBPP::brReplace);
	//svc->StartRestore(BkName, DbName, 0, IBPP::brVerbose);
	//svc->StartRestore(BkName, DbName);

	//char* line;
	//while ((line = (char*)svc->WaitMsg()) != 0) printf("%s\n", line);
	svc->Wait();

	printf(_("           Manage users\n"));
	svc->RemoveUser("EPOCMAN");
	svc->AddUser("EPOCMAN", "test", "Olivier", "Gilles", "Mascia");
	svc->ModifyUser("EPOCMAN", "", "Benoit", "", "");
	printf("               \r");
	std::vector<std::string> users;
	svc->ListUsers(users);
	printf("           All users : ");
	for (unsigned int i = 0; i < users.size(); i++)
	{
		printf("%s", users[i].c_str());
		if (i < users.size()-1) printf(", ");
	}
	printf("\n");

	// Connecting two users
	db1 = IBPP::DatabaseFactory(ServerName, DbName, "EPOCMAN", "test");
	db1->Connect();
	IBPP::Database db2 = IBPP::DatabaseFactory(ServerName, DbName, UserName, Password);
	db2->Connect();

	// Checking their names
	db1->Users(users);
	if (users.size() != 2)
	{
		_Success = false;
		printf(_("           Expected 2 users connected, found %d.\n"), (int)users.size());
	}
	printf("           Connected users : ");
	for (unsigned int i = 0; i < users.size(); i++)
	{
		printf("%s", users[i].c_str());
		if (i < users.size()-1) printf(", ");
	}
	printf("\n");
}

void Test::Test7(void)
{
	printf(_("Test 7 --- Mass delete, AffectedRows() Statistics() and Counts()\n"));

	IBPP::Database db1;
	db1 = IBPP::DatabaseFactory(ServerName, DbName, UserName, Password);
	db1->Connect();

	IBPP::Transaction tr1 = IBPP::TransactionFactory(db1, IBPP::amWrite);
	tr1->Start();

	IBPP::Statement st1 = IBPP::StatementFactory(db1, tr1);
	st1->Prepare("delete from test");
	st1->Execute();
	if (st1->AffectedRows() != 100)
	{
		_Success = false;
		printf(_("           Statement::AffectedRows() not working.\n"
			"           Returned %d when 100 was expected.\n"), st1->AffectedRows());
	}
	else printf(_("           Affected rows : %d\n"), st1->AffectedRows());

	tr1->Commit();

	int Fetches, Marks, Reads, Writes;

	db1->Statistics(&Fetches, &Marks, &Reads, &Writes);
	printf("           Fetches   : %d\n", Fetches);
	printf("           Marks     : %d\n", Marks);
	printf("           Reads     : %d\n", Reads);
	printf("           Writes    : %d\n", Writes);

	int Inserts, Updates, Deletes, ReadIdx, ReadSeq;

	db1->Counts(&Inserts, &Updates, &Deletes, &ReadIdx, &ReadSeq);
	printf("           Inserts   : %d\n", Inserts);
	printf("           Updates   : %d\n", Updates);
	printf("           Deletes   : %d\n", Deletes);
	printf("           ReadIdx   : %d\n", ReadIdx);
	printf("           ReadSeq   : %d\n", ReadSeq);
}

class EventCatch : public IBPP::EventInterface
{
	virtual void ibppEventHandler(IBPP::IDatabase*, const std::string& name, int count)
	{
		printf(_("           *** Event %s triggered, count = %d ***\n"), name.c_str(), count);
	}
};

void Test::Test8(void)
{
	printf(_("Test 8 --- Events interface\n"));

	IBPP::Database db1;
	db1 = IBPP::DatabaseFactory(ServerName, DbName, UserName, Password);
	db1->Connect();

	EventCatch catcher;

	// The following transaction configuration values are the defaults and
	// those parameters could have as well be omitted to simplify writing.
	IBPP::Transaction tr1 = IBPP::TransactionFactory(db1,
							IBPP::amWrite, IBPP::ilConcurrency, IBPP::lrWait);
	tr1->Start();

	IBPP::Statement st1 = IBPP::StatementFactory(db1, tr1);
	printf(_("           Adding a trigger to test database...\n"));
	st1->ExecuteImmediate(
        "CREATE TRIGGER TEST_TRIGGER FOR TEST ACTIVE AFTER INSERT AS\n"
        "BEGIN\n"
		"	POST_EVENT 'INSERT';\n"
        "END"   );
	tr1->Commit();

	// Let's "sink" the 'INSERT' event among a big set of 200 events which would
	// have been completely unsupported by the FB C-API while the engine do
	// support it nicely (through IBPP).

	printf(_("           Registering 200 events (!)\n"));
	int i;
	char event[15];
	for (i = 1; i <= 100; i++)
	{
		sprintf(event, "EVENTNUMBER%3.3d", i);
		db1->DefineEvent(event, &catcher);
	}

	db1->DefineEvent("INSERT", &catcher);

	for (i = 101; i <= 200; i++)
	{
		sprintf(event, "EVENTNUMBER%3.3d", i);
		db1->DefineEvent(event, &catcher);
	}

	printf(_("           Inserting 2 records, that should trigger 'INSERT' event...\n"));
	tr1->Start();
	st1->ExecuteImmediate("INSERT INTO TEST(N2) VALUES(1)");
	st1->ExecuteImmediate("INSERT INTO TEST(N2) VALUES(1)");
	printf("           Commit...\n");
	tr1->Commit();
	
	printf(_("           First immediate call to DispatchEvents()\n"));
	db1->DispatchEvents();

	printf(_("           Sleeping 2 sec...\n"));
	Sleep(2000);

	printf(_("           Adding an event\n"));
	db1->DefineEvent("FOURTH", &catcher);

	printf(_("           Second call to DispatchEvents()\n"));
	db1->DispatchEvents();

	printf(_("           Inserting 3 records, that should trigger 'INSERT' event...\n"));
	tr1->Start();
	st1->ExecuteImmediate("INSERT INTO TEST(N2) VALUES(1)");
	st1->ExecuteImmediate("INSERT INTO TEST(N2) VALUES(1)");
	st1->ExecuteImmediate("INSERT INTO TEST(N2) VALUES(1)");

	printf(_("           Third call to DispatchEvents (commit not done)...\n"));
	db1->DispatchEvents();

	printf(_("           Now committing (events should only trigger after commit)...\n"));
	tr1->Commit();

	printf(_("           Series of 20 calls to DispatchEvents(),\n"));
	printf(_("           with a 0.050 sec sleep after each\n"));
	for (i = 0; i < 20; i++)
	{
		db1->DispatchEvents();
		Sleep(50);
	}
	printf("\n");

	printf(_("           Start new transaction, trigger the event, drop it then dispatch...\n"));
	tr1->Start();
	st1->ExecuteImmediate("INSERT INTO TEST(N2) VALUES(1)");
	st1->ExecuteImmediate("INSERT INTO TEST(N2) VALUES(1)");
	st1->ExecuteImmediate("INSERT INTO TEST(N2) VALUES(1)");
	db1->DropEvent("INSERT");
	printf(_("           Series of 20 calls to DispatchEvents(),\n"));
	printf(_("           with a 0.050 sec sleep after each\n"));
	for (i = 0; i < 20; i++)
	{
		db1->DispatchEvents();
		Sleep(50);
	}
	printf("\n");
	printf(_("           The event should not have been caught.\n"));

	db1->Drop();
}

Test::Test(int argc, char* argv[])
{
	if (argc == 2 && argv[1] != 0 && strcmp(argv[1], "speed") == 0)
		_WriteMode = 1;
	else if (argc == 2 && argv[1] != 0 && strcmp(argv[1], "safety") == 0)
		_WriteMode = 2;
	else
		_WriteMode = 0;

	_Success = true;
}

Test::~Test()
{
}

int main(int argc, char* argv[])
{
	Test T(argc, argv);

	if (! IBPP::CheckVersion(IBPP::Version))
	{
		printf(_("\nThis program got linked to an incompatible version of the library.\n"
			"Can't execute safely.\n"));
		return 2;
	}

	T.RunTests();

	return T.PrintResult();
}

//	Eof