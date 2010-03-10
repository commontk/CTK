-- 
-- A simple SQLITE3 database schema for modelling locally stored DICOM files 
-- 
-- Note: the semicolon at the end is necessary for the simple parser to separate
--       the statements since the SQlite driver does not handle multiple
--       commands per QSqlQuery::exec call!
-- ;

DROP TABLE IF EXISTS 'Images' ;
DROP TABLE IF EXISTS 'Patients' ;
DROP TABLE IF EXISTS 'Series' ;
DROP TABLE IF EXISTS 'Studies' ;

CREATE TABLE 'Images' (
  'Filename' VARCHAR(1024) NOT NULL ,
  'SeriesInstanceUID' VARCHAR(255) NOT NULL ,
  PRIMARY KEY ('Filename') );
CREATE TABLE 'Patients' (
  'UID' INTEGER PRIMARY KEY AUTOINCREMENT,
  'PatientsName' VARCHAR(255) NULL ,
  'PatientID' VARCHAR(255) NULL ,
  'PatientsBirthDate' DATE NULL ,
  'PatientsBirthTime' TIME NULL ,
  'PatientsSex' varchar(1) NULL ,
  'PatientsComments' VARCHAR(255) NULL );
CREATE TABLE 'Series' (
  'SeriesInstanceUID' VARCHAR(255) NOT NULL ,
  'StudyInstanceUID' VARCHAR(45) NOT NULL ,
  'SeriesNumber' INT NULL ,
  'SeriesDate' DATE NULL ,
  'SeriesTime' VARCHAR(20) NULL ,
  'SeriesDescription' VARCHAR(255) NULL ,
  'BodyPartExamined' VARCHAR(255) NULL ,
  'FrameOfReferenceUID' VARCHAR(255) NULL ,
  'AcquisitionNumber' INT NULL ,
  'ContrastAgent' VARCHAR(255) NULL ,
  'ScanningSequence' VARCHAR(45) NULL ,
  'EchoNumber' INT NULL ,
  'TemporalPosition' INT NULL ,
  PRIMARY KEY ('SeriesInstanceUID') );
CREATE TABLE 'Studies' (
  'StudyInstanceUID' VARCHAR(255) NOT NULL ,
  'PatientsUID' INT NOT NULL ,
  'StudyID' VARCHAR(255) NULL ,
  'StudyDate' DATE NULL ,
  'StudyTime' VARCHAR(20) NULL ,
  'AccessionNumber' VARCHAR(255) NULL ,
  'ModalitiesInStudy' VARCHAR(255) NULL ,
  'ReferringPhysician' VARCHAR(255) NULL ,
  'StudyDescription' VARCHAR(255) NULL ,
  PRIMARY KEY ('StudyInstanceUID') );

