#ifndef __CINT__
#include <TSQLServer.h>
#include <TSQLResult.h>
#include <TSQLRow.h>
#endif
   
void select()
{

   gSystem->Load("/apps/lib/libz");


   TSQLServer *db = TSQLServer::Connect("mysql://alquds.jlab.org","dbmanager", "parity");

   printf("Server info: %s\n", db->ServerInfo());
   
   TSQLRow *row;
   TSQLResult *res;
   
   // list databases available on server
   printf("\nList all databases on server %s\n", db->GetHost());
   res = db->GetDataBases();
   while ((row = res->Next())) {
      printf("%s\n", row->GetField(0));
      delete row;
   }
   delete res;

   // list tables in database "pandb" (the permission tables)
   printf("\nList all tables in database \"pandb\" on server %s\n",
          db->GetHost());
   res = db->GetTables("pandb");
   while ((row = res->Next())) {
      printf("%s\n", row->GetField(0));
      delete row;
   }
   delete res;
   
   // list columns in table "RunIndex " in database "mysql"
   printf("\nList all columns in table \"RunIndex \" in database \"pandb\" on server %s\n",
          db->GetHost());
   res = db->GetColumns("pandb", "RunIndex ");
   while ((row = res->Next())) {
      printf("%s\n", row->GetField(0));
      delete row;
   }
   delete res;

   // start timer
   TStopwatch timer;
   timer.Start();

   // query database and print results
   const char *sql = "select RunNumber,itemId from pandb.RunIndex";
   
   res = db->Query(sql);

   int nrows = res->GetRowCount();
   printf("\nGot %d rows in result\n", nrows);
   
   int nfields = res->GetFieldCount();
   for (int i = 0; i < nfields; i++)
      printf("%40s", res->GetFieldName(i));
   printf("\n");
   for (int i = 0; i < nfields*40; i++)
      printf("=");
   printf("\n");
   
   for (int i = 0; i < nrows; i++) {
      row = res->Next();
      for (int j = 0; j < nfields; j++) {
         printf("%40s", row->GetField(j));
      }
      printf("\n");
      delete row;
   }
   
   delete res;
   delete db;

   // stop timer and print results
   timer.Stop();
   Double_t rtime = timer.RealTime();
   Double_t ctime = timer.CpuTime();

   printf("\nRealTime=%f seconds, CpuTime=%f seconds\n", rtime, ctime);
}
