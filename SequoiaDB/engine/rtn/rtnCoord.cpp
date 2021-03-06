/*******************************************************************************

   Copyright (C) 2011-2014 SequoiaDB Ltd.

   This program is free software: you can redistribute it and/or modify
   it under the term of the GNU Affero General Public License, version 3,
   as published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warrenty of
   MARCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU Affero General Public License for more details.

   You should have received a copy of the GNU Affero General Public License
   along with this program. If not, see <http://www.gnu.org/license/>.

   Source File Name = rtnCoord.cpp

   Descriptive Name = Runtime Coord

   When/how to use: this program may be used on binary and text-formatted
   versions of runtime component. This file contains code logic for
   command factory on coordinator node.

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================

   Last Changed =

*******************************************************************************/

#include "rtnCoord.hpp"
#include "rtnCoordCommands.hpp"
#include "rtnCoordSnapshot.hpp"
#include "rtnCoordList.hpp"
#include "rtnCoordDataCommands.hpp"
#include "rtnCoordNodeCommands.hpp"
#include "rtnCoordDCCommands.hpp"
#include "rtnCoordOperator.hpp"
#include "rtnCoordAuth.hpp"
#include "rtnCoordAuthCrt.hpp"
#include "rtnCoordAuthDel.hpp"
#include "rtnCoordInsert.hpp"
#include "rtnCoordQuery.hpp"
#include "rtnCoordDelete.hpp"
#include "rtnCoordUpdate.hpp"
#include "rtnCoordInterrupt.hpp"
#include "rtnCoordTransaction.hpp"
#include "rtnCoordSql.hpp"
#include "rtnCoordAggregate.hpp"
#include "rtnCoordLob.hpp"
#include "pdTrace.hpp"
#include "rtnTrace.hpp"

namespace engine
{


   /*
      MAP COMMANDS
   */
   RTN_COORD_CMD_BEGIN
   RTN_COORD_CMD_ADD( COORD_CMD_BACKUP_OFFLINE, rtnCoordBackupOffline, FALSE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_REMOVE_BACKUP, rtnCoordRemoveBackup, FALSE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_CREATECOLLECTIONSPACE, rtnCoordCMDCreateCollectionSpace, FALSE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_CREATECOLLECTION, rtnCoordCMDCreateCollection, FALSE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_ALTERCOLLECTION, rtnCoordCMDAlterCollection, FALSE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_DROPCOLLECTION, rtnCoordCMDDropCollection, FALSE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_DROPCOLLECTIONSPACE, rtnCoordCMDDropCollectionSpace, FALSE ) ;
   /* List */
   RTN_COORD_CMD_ADD( COORD_CMD_LIST_BACKUPS, rtnCoordListBackup, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_LISTGROUPS,  rtnCoordCMDListGroups, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_LISTCOLLECTIONSPACES, rtnCoordCMDListCollectionSpace, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_LISTCOLLECTIONS, rtnCoordCMDListCollection, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_LIST_SESSIONS, rtnCoordCMDListSessions, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_LIST_SESSIONS_CUR, rtnCoordCMDListSessionsCur, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_LIST_CONTEXTS, rtnCoordCMDListContexts, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_LIST_CONTEXTS_CUR, rtnCoordCMDListContextsCur, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_LISTUSERS, rtnCoordCMDListUser, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_LIST_TRANS, rtnCoordListTrans, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_LIST_TRANS_CUR, rtnCoordListTransCur, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_LIST_TASKS, rtnCoordCmdListTask, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_LIST_DOMAINS, rtnCoordCMDListDomains, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_LIST_PROCEDURES, rtnCoordCMDListProcedures, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_LIST_CS_IN_DOMAIN, rtnCoordCMDListCSInDomain, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_LIST_CL_IN_DOMAIN, rtnCoordCMDListCLInDomain, TRUE ) ;
   /* End list */
   /* Virtual list */
   RTN_COORD_CMD_ADD( COORD_CMD_LISTBACKUPINTR, rtnCoordListBackupIntr, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_LISTGROUPINTR,  rtnCoordCMDListGroups, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_LISTCSINTR, rtnCoordCMDListCollectionSpaceIntr, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_LISTCLINTR, rtnCoordCMDListCollectionIntr, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_LISTCTXINTR, rtnCoordCMDListIntrBase, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_LISTCTXCURINTR, rtnCoordCMDListCurIntrBase, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_LISTSESSINTR, rtnCoordCMDListIntrBase, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_LISTSESSCURINTR, rtnCoordCMDListCurIntrBase, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_LISTUSERINTR, rtnCoordCMDListUser, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_LISTTRANSCURINTR, rtnCoordListTransCurIntr, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_LISTTRANSINTR, rtnCoordListTransIntr, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_LISTTASKINTR, rtnCoordCmdListTask, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_LISTDOMAININTR, rtnCoordCMDListDomains, TRUE ) ;
   /* End virtual list */
   /* Snapshot */
   RTN_COORD_CMD_ADD( COORD_CMD_SNAPSHOTDATABASE, rtnCoordCMDSnapshotDataBase, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_SNAPSHOTSYSTEM, rtnCoordCMDSnapshotSystem, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_SNAPSHOTSESSIONS, rtnCoordCMDSnapshotSessions, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_SNAPSHOTSESSIONSCUR, rtnCoordCMDSnapshotSessionsCur, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_SNAPSHOTCONTEXTS, rtnCoordCMDSnapshotContexts, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_SNAPSHOTCONTEXTSCUR, rtnCoordCMDSnapshotContextsCur, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_SNAPSHOTRESET, rtnCoordCmdSnapshotReset, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_SNAPSHOTCOLLECTIONS, rtnCoordCMDSnapshotCollections, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_SNAPSHOTCOLLECTIONSPACES, rtnCoordCMDSnapshotSpaces, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_SNAPSHOTCATALOG, rtnCoordCMDSnapshotCata, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_SNAPSHOTTRANS, rtnCoordSnapshotTrans, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_SNAPSHOTTRANSCUR, rtnCoordSnapshotTransCur, TRUE ) ;
   /* End snapshot */
   /* Virtual snapshot */
   RTN_COORD_CMD_ADD( COORD_CMD_SNAPSHOTDBINTR, rtnCoordCMDSnapshotIntrBase, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_SNAPSHOTSYSINTR, rtnCoordCMDSnapshotIntrBase, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_SNAPSHOTCLINTR, rtnCoordCMDSnapshotIntrBase, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_SNAPSHOTCSINTR, rtnCoordCMDSnapshotIntrBase, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_SNAPSHOTCTXINTR, rtnCoordCMDSnapshotIntrBase, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_SNAPSHOTCTXCURINTR, rtnCoordCMDSnapshotCurIntrBase, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_SNAPSHOTSESSINTR, rtnCoordCMDSnapshotIntrBase, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_SNAPSHOTSESSCURINTR, rtnCoordCMDSnapshotCurIntrBase, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_SNAPSHOTCATAINTR, rtnCoordCMDSnapshotCata, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_SNAPSHOTTRANSCURINTR, rtnCoordSnapshotTransCurIntr, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_SNAPSHOTTRANSINTR, rtnCoordSnapshotTransIntr, TRUE ) ;
   /* End virtual snapshot */
   RTN_COORD_CMD_ADD( COORD_CMD_TESTCOLLECTIONSPACE, rtnCoordCMDTestCollectionSpace, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_TESTCOLLECTION, rtnCoordCMDTestCollection, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_CREATEGROUP, rtnCoordCMDCreateGroup, FALSE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_REMOVEGROUP, rtnCoordCMDRemoveGroup, FALSE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_ACTIVEGROUP, rtnCoordCMDActiveGroup, FALSE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_CREATENODE, rtnCoordCMDCreateNode, FALSE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_REMOVENODE, rtnCoordCMDRemoveNode, FALSE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_UPDATENODE, rtnCoordCMDUpdateNode, FALSE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_CREATEINDEX, rtnCoordCMDCreateIndex, FALSE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_DROPINDEX, rtnCoordCMDDropIndex, FALSE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_STARTUPNODE, rtnCoordCMDStartupNode, FALSE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_SHUTDOWNNODE, rtnCoordCMDShutdownNode, FALSE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_SHUTDOWNGROUP, rtnCoordCMDShutdownGroup, FALSE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_GETCOUNT, rtnCoordCMDGetCount, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_GETDATABLOCKS, rtnCoordCMDGetDatablocks, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_GETQUERYMETA, rtnCoordCMDGetQueryMeta, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_SPLIT, rtnCoordCMDSplit, FALSE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_WAITTASK, rtnCoordCmdWaitTask, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_GETINDEXES, rtnCoordCMDGetIndexes, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_CREATECATAGROUP, rtnCoordCMDCreateCataGroup, FALSE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_TRACESTART, rtnCoordCMDTraceStart, FALSE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_TRACESTOP, rtnCoordCMDTraceStop, FALSE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_TRACERESUME, rtnCoordCMDTraceResume, FALSE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_TRACESTATUS, rtnCoordCMDTraceStatus, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_EXPCONFIG, rtnCoordCMDExpConfig, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_CRT_PROCEDURE, rtnCoordCMDCrtProcedure, FALSE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_EVAL, rtnCoordCMDEval, FALSE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_RM_PROCEDURE, rtnCoordCMDRmProcedure, FALSE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_DEFAULT, rtnCoordDefaultCommand, FALSE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_LINK, rtnCoordCMDLinkCollection, FALSE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_UNLINK, rtnCoordCMDUnlinkCollection, FALSE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_CANCEL_TASK, rtnCoordCmdCancelTask, FALSE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_SET_SESS_ATTR, rtnCoordCMDSetSessionAttr, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_GET_SESS_ATTR, rtnCoordCMDGetSessionAttr, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_CREATE_DOMAIN, rtnCoordCMDCreateDomain, FALSE) ;
   RTN_COORD_CMD_ADD( COORD_CMD_DROP_DOMAIN, rtnCoordCMDDropDomain, FALSE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_ALTER_DOMAIN, rtnCoordCMDAlterDomain, FALSE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_INVALIDATE_CACHE, rtnCoordCMDInvalidateCache, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_LIST_LOBS, rtnCoordCMDListLobs, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_ALTER_DC, rtnCoordAlterDC, FALSE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_GETDCINFO, rtnCoordGetDCInfo, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_REELECT, rtnCoordCMDReelection, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_TRUNCATE, rtnCoordCMDTruncate, FALSE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_SYNC_DB, rtnCoordCMDSyncDB, FALSE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_FORCE_SESSION, rtnCoordForceSession, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_LOAD_COLLECTIONSPACE, rtnCoordCmdLoadCS, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_UNLOAD_COLLECTIONSPACE, rtnCoordCmdUnloadCS, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_SET_PDLEVEL, rtnCoordSetPDLevel, TRUE ) ;
   RTN_COORD_CMD_ADD( COORD_CMD_RELOAD_CONFIG, rtnCoordReloadConf, TRUE ) ;
   RTN_COORD_CMD_END

   /*
      MAP OPERATIONS
   */
   RTN_COORD_OP_BEGIN
   RTN_COORD_OP_ADD( MSG_BS_INSERT_REQ, rtnCoordInsert, FALSE ) ;
   RTN_COORD_OP_ADD( MSG_BS_QUERY_REQ, rtnCoordQuery, TRUE ) ;
   RTN_COORD_OP_ADD( MSG_BS_DELETE_REQ, rtnCoordDelete, FALSE ) ;
   RTN_COORD_OP_ADD( MSG_BS_UPDATE_REQ, rtnCoordUpdate, FALSE ) ;
   RTN_COORD_OP_ADD( MSG_BS_AGGREGATE_REQ, rtnCoordAggregate, TRUE ) ;
   RTN_COORD_OP_ADD( MSG_BS_INTERRUPTE, rtnCoordInterrupt, TRUE ) ;
   RTN_COORD_OP_ADD( MSG_AUTH_VERIFY_REQ, rtnCoordAuth, TRUE ) ;
   RTN_COORD_OP_ADD( MSG_AUTH_CRTUSR_REQ, rtnCoordAuthCrt, FALSE ) ;
   RTN_COORD_OP_ADD( MSG_AUTH_DELUSR_REQ, rtnCoordAuthDel, FALSE ) ;
   RTN_COORD_OP_ADD( MSG_BS_TRANS_BEGIN_REQ, rtnCoordTransBegin, FALSE ) ;
   RTN_COORD_OP_ADD( MSG_BS_TRANS_COMMIT_REQ, rtnCoordTransCommit, FALSE ) ;
   RTN_COORD_OP_ADD( MSG_BS_TRANS_ROLLBACK_REQ, rtnCoordTransRollback, FALSE ) ;
   RTN_COORD_OP_ADD( MSG_BS_SQL_REQ, rtnCoordSql, FALSE ) ;
   RTN_COORD_OP_ADD( MSG_BS_MSG_REQ, rtnCoordMsg, TRUE ) ;
   RTN_COORD_OP_ADD( MSG_BS_LOB_OPEN_REQ, rtnCoordOpenLob, FALSE ) ;
   RTN_COORD_OP_ADD( MSG_BS_LOB_WRITE_REQ, rtnCoordWriteLob, FALSE ) ;
   RTN_COORD_OP_ADD( MSG_BS_LOB_READ_REQ, rtnCoordReadLob, TRUE ) ;
   RTN_COORD_OP_ADD( MSG_BS_LOB_CLOSE_REQ, rtnCoordCloseLob, TRUE ) ;
   RTN_COORD_OP_ADD( MSG_BS_LOB_REMOVE_REQ, rtnCoordRemoveLob, FALSE ) ;
   RTN_COORD_OP_ADD( MSG_NULL, rtnCoordOperatorDefault, FALSE ) ;
   RTN_COORD_OP_END

   rtnCoordProcesserFactory::rtnCoordProcesserFactory()
   {
      addCommand();
      addOperator();
   }

   PD_TRACE_DECLARE_FUNCTION ( SDB_RTNCOPROFAC_RTNCOPROFAC, "rtnCoordProcesserFactory::~rtnCoordProcesserFactory" )
   rtnCoordProcesserFactory::~rtnCoordProcesserFactory()
   {
      PD_TRACE_ENTRY ( SDB_RTNCOPROFAC_RTNCOPROFAC ) ;
      COORD_CMD_MAP::iterator iter;
      iter = _cmdMap.begin();
      while ( iter != _cmdMap.end() )
      {
         SDB_OSS_DEL iter->second;
         _cmdMap.erase( iter++ );
      }
      _cmdMap.clear();

      COORD_OP_MAP::iterator iterOp;
      iterOp = _opMap.begin();
      while( iterOp != _opMap.end() )
      {
         SDB_OSS_DEL iterOp->second;
         _opMap.erase( iterOp++ );
      }
      PD_TRACE_EXIT ( SDB_RTNCOPROFAC_RTNCOPROFAC ) ;
   }

   PD_TRACE_DECLARE_FUNCTION ( SDB_RTNCOPROFAC_GETOP, "rtnCoordProcesserFactory::getOperator" )
   rtnCoordOperator * rtnCoordProcesserFactory::getOperator( SINT32 opCode )
   {
      PD_TRACE_ENTRY ( SDB_RTNCOPROFAC_GETOP ) ;
      COORD_OP_MAP::iterator iter;
      iter = _opMap.find ( opCode );
      if ( _opMap.end() == iter )
      {
         iter = _opMap.find ( MSG_NULL );
      }
      PD_TRACE_EXIT ( SDB_RTNCOPROFAC_GETOP ) ;
      return iter->second;
   }

   PD_TRACE_DECLARE_FUNCTION ( SDB_RTNCOPROFAC_GETCOMPRO1, "rtnCoordProcesserFactory::getCommandProcesser" )
   rtnCoordCommand * rtnCoordProcesserFactory::getCommandProcesser(const MsgOpQuery *pQuery)
   {
      PD_TRACE_ENTRY ( SDB_RTNCOPROFAC_GETCOMPRO1 ) ;
      SDB_ASSERT ( pQuery, "pQuery can't be NULL" ) ;
      rtnCoordCommand *pProcesser = NULL;
      do
      {
         if ( MSG_BS_QUERY_REQ == pQuery->header.opCode )
         {
            if ( pQuery->nameLength > 0 )
            {
               COORD_CMD_MAP::iterator iter;
               iter = _cmdMap.find( pQuery->name );
               if ( iter != _cmdMap.end() )
               {
                  pProcesser = iter->second;
               }
            }
         }
         if ( NULL == pProcesser )
         {
            COORD_CMD_MAP::iterator iter;
            iter = _cmdMap.find( COORD_CMD_DEFAULT );
            if ( iter != _cmdMap.end() )
            {
               pProcesser = iter->second;
            }
         }
      }while ( FALSE );
      PD_TRACE_EXIT ( SDB_RTNCOPROFAC_GETCOMPRO1 ) ;
      return pProcesser;
   }

   PD_TRACE_DECLARE_FUNCTION ( SDB_RTNCOPROFAC_GETCOMPRO2, "rtnCoordProcesserFactory::getCommandProcesser" )
   rtnCoordCommand * rtnCoordProcesserFactory::getCommandProcesser(const char *pCmd)
   {
      PD_TRACE_ENTRY ( SDB_RTNCOPROFAC_GETCOMPRO2 ) ;
      SDB_ASSERT ( pCmd, "pCmd can't be NULL" ) ;
      rtnCoordCommand *pProcesser = NULL;
      do
      {
         COORD_CMD_MAP::iterator iter;
         iter = _cmdMap.find( pCmd );
         if ( iter != _cmdMap.end() )
         {
            pProcesser = iter->second;
         }
      }while ( FALSE );
      PD_TRACE_EXIT ( SDB_RTNCOPROFAC_GETCOMPRO2 ) ;
      return pProcesser;
   }
}
