--
-- A simple SQLITE3 database schema for temporary storage of query responses
--
-- Note: the semicolon at the end is necessary for the simple parser to separate
--       the statements since the SQlite driver does not handle multiple
--       commands per QSqlQuery::exec call!
-- Note: be sure to update ctkDICOMDatabase and SchemaInfo Version
--       whenever you make a change to this schema
-- ;

DROP TABLE IF EXISTS 'SchemaInfo' ;
DROP TABLE IF EXISTS 'Images' ;
DROP TABLE IF EXISTS 'Patients' ;
DROP TABLE IF EXISTS 'Series' ;
DROP TABLE IF EXISTS 'Studies' ;
DROP TABLE IF EXISTS 'ColumnDisplayProperties' ;
DROP TABLE IF EXISTS 'Directories' ;

DROP INDEX IF EXISTS 'ImagesFilenameIndex' ;
DROP INDEX IF EXISTS 'ImagesSeriesIndex' ;
DROP INDEX IF EXISTS 'SeriesStudyIndex' ;
DROP INDEX IF EXISTS 'StudiesPatientIndex' ;

CREATE TABLE 'SchemaInfo' ( 'Version' VARCHAR(1024) NOT NULL );
INSERT INTO 'SchemaInfo' VALUES('0.6.2');

CREATE TABLE 'Images' (
  'SOPInstanceUID' VARCHAR(64) NOT NULL,
  'Filename' VARCHAR(1024) NOT NULL ,
  'SeriesInstanceUID' VARCHAR(64) NOT NULL ,
  'InsertTimestamp' VARCHAR(20) NOT NULL ,
  'DisplayedFieldsUpdatedTimestamp' DATETIME NULL ,
  PRIMARY KEY ('SOPInstanceUID') );
CREATE TABLE 'Patients' (
  'UID' INTEGER PRIMARY KEY AUTOINCREMENT,
  'PatientsName' VARCHAR(255) NULL ,
  'PatientID' VARCHAR(255) NULL ,
  'PatientsBirthDate' DATE NULL ,
  'PatientsBirthTime' TIME NULL ,
  'PatientsSex' VARCHAR(1) NULL ,
  'PatientsAge' VARCHAR(10) NULL ,
  'PatientsComments' VARCHAR(255) NULL ,
  'InsertTimestamp' VARCHAR(20) NOT NULL ,
  'DisplayedPatientsName' VARCHAR(255) NULL ,
  'DisplayedNumberOfStudies' INT NULL ,
  'DisplayedFieldsUpdatedTimestamp' DATETIME NULL );
CREATE TABLE 'Studies' (
  'StudyInstanceUID' VARCHAR(64) NOT NULL ,
  'PatientsUID' INT NOT NULL ,
  'StudyID' VARCHAR(255) NULL ,
  'StudyDate' DATE NULL ,
  'StudyTime' VARCHAR(20) NULL ,
  'StudyDescription' VARCHAR(255) NULL ,
  'AccessionNumber' VARCHAR(255) NULL ,
  'ModalitiesInStudy' VARCHAR(255) NULL ,
  'InstitutionName' VARCHAR(255) NULL ,
  'ReferringPhysician' VARCHAR(255) NULL ,
  'PerformingPhysiciansName' VARCHAR(255) NULL ,
  'InsertTimestamp' VARCHAR(20) NOT NULL ,
  'DisplayedNumberOfSeries' INT NULL ,
  'DisplayedFieldsUpdatedTimestamp' DATETIME NULL ,
  PRIMARY KEY ('StudyInstanceUID') );
CREATE TABLE 'Series' (
  'SeriesInstanceUID' VARCHAR(64) NOT NULL ,
  'StudyInstanceUID' VARCHAR(64) NOT NULL ,
  'SeriesNumber' INT NULL ,
  'SeriesDate' DATE NULL ,
  'SeriesTime' VARCHAR(20) NULL ,
  'SeriesDescription' VARCHAR(255) NULL ,
  'Modality' VARCHAR(20) NULL ,
  'BodyPartExamined' VARCHAR(255) NULL ,
  'FrameOfReferenceUID' VARCHAR(64) NULL ,
  'AcquisitionNumber' INT NULL ,
  'ContrastAgent' VARCHAR(255) NULL ,
  'ScanningSequence' VARCHAR(45) NULL ,
  'EchoNumber' INT NULL ,
  'TemporalPosition' INT NULL ,
  'InsertTimestamp' VARCHAR(20) NOT NULL ,
  'DisplayedCount' INT NULL ,
  'DisplayedSize' VARCHAR(20) NULL ,
  'DisplayedNumberOfFrames' INT NULL ,
  'DisplayedFieldsUpdatedTimestamp' DATETIME NULL ,
  PRIMARY KEY ('SeriesInstanceUID') );

CREATE UNIQUE INDEX IF NOT EXISTS 'ImagesFilenameIndex' ON 'Images' ('Filename');
CREATE INDEX IF NOT EXISTS 'ImagesSeriesIndex' ON 'Images' ('SeriesInstanceUID');
CREATE INDEX IF NOT EXISTS 'SeriesStudyIndex' ON 'Series' ('StudyInstanceUID');
CREATE INDEX IF NOT EXISTS 'StudiesPatientIndex' ON 'Studies' ('PatientsUID');

CREATE TABLE 'Directories' (
  'Dirname' VARCHAR(1024) ,
  PRIMARY KEY ('Dirname') );

CREATE TABLE 'ColumnDisplayProperties' (
  'TableName' VARCHAR(64) NOT NULL,
  'FieldName' VARCHAR(64) NOT NULL ,
  'DisplayedName' VARCHAR(255) NULL ,
  'Visibility' INT NULL DEFAULT 1 ,
  'Weight' INT NULL ,
  'Format' VARCHAR(255) NULL ,
  PRIMARY KEY ('TableName', 'FieldName') );

INSERT INTO 'ColumnDisplayProperties' VALUES('Patients', 'UID',                             '',                     0, 0, '');
INSERT INTO 'ColumnDisplayProperties' VALUES('Patients', 'PatientsName',                    'Patient name',         1, 0, '{"sort": "ascending", "resizeMode":"stretch"}');
INSERT INTO 'ColumnDisplayProperties' VALUES('Patients', 'PatientID',                       'Patient ID',           1, 1, '');
INSERT INTO 'ColumnDisplayProperties' VALUES('Patients', 'PatientsBirthDate',               'Birth date',           1, 2, '{"resizeMode":"resizeToContents"}');
INSERT INTO 'ColumnDisplayProperties' VALUES('Patients', 'PatientsBirthTime',               'Birth time',           0, 0, '');
INSERT INTO 'ColumnDisplayProperties' VALUES('Patients', 'PatientsSex',                     'Sex',                  1, 3, '{"resizeMode":"resizeToContents"}');
INSERT INTO 'ColumnDisplayProperties' VALUES('Patients', 'PatientsAge',                     'Age',                  0, 0, '');
INSERT INTO 'ColumnDisplayProperties' VALUES('Patients', 'PatientsComments',                'Comments',             0, 0, '');
INSERT INTO 'ColumnDisplayProperties' VALUES('Patients', 'InsertTimestamp',                 'Date added',           0, 0, '');
INSERT INTO 'ColumnDisplayProperties' VALUES('Patients', 'DisplayedPatientsName',           'Patient name',         0, 0, '');
INSERT INTO 'ColumnDisplayProperties' VALUES('Patients', 'DisplayedNumberOfStudies',        'Studies',              0, 0, '');
INSERT INTO 'ColumnDisplayProperties' VALUES('Patients', 'DisplayedFieldsUpdatedTimestamp', '',                     0, 0, '');

INSERT INTO 'ColumnDisplayProperties' VALUES('Studies',  'StudyInstanceUID',                '',                     0, 0, '');
INSERT INTO 'ColumnDisplayProperties' VALUES('Studies',  'PatientsUID',                     '',                     0, 0, '');
INSERT INTO 'ColumnDisplayProperties' VALUES('Studies',  'StudyID',                         'Study ID',             1, 2, '');
INSERT INTO 'ColumnDisplayProperties' VALUES('Studies',  'StudyDate',                       'Study date',           1, 1, '{"resizeMode":"resizeToContents", "sort": "ascending"}');
INSERT INTO 'ColumnDisplayProperties' VALUES('Studies',  'StudyTime',                       'Study time',           0, 0, '');
INSERT INTO 'ColumnDisplayProperties' VALUES('Studies',  'StudyDescription',                'Study description',    1, 3, '{"resizeMode":"stretch"}');
INSERT INTO 'ColumnDisplayProperties' VALUES('Studies',  'AccessionNumber',                 'Accession #',          0, 0, '');
INSERT INTO 'ColumnDisplayProperties' VALUES('Studies',  'ModalitiesInStudy',               'Modalities',           0, 0, '');
INSERT INTO 'ColumnDisplayProperties' VALUES('Studies',  'InstitutionName',                 'Institution',          0, 0, '');
INSERT INTO 'ColumnDisplayProperties' VALUES('Studies',  'ReferringPhysician',              'Referring physician',  0, 0, '');
INSERT INTO 'ColumnDisplayProperties' VALUES('Studies',  'PerformingPhysiciansName',        'Performing physician', 0, 0, '');
INSERT INTO 'ColumnDisplayProperties' VALUES('Studies',  'InsertTimestamp',                 'Date added',           0, 5, '');
INSERT INTO 'ColumnDisplayProperties' VALUES('Studies',  'DisplayedNumberOfSeries',         'Series',               0, 4, '');
INSERT INTO 'ColumnDisplayProperties' VALUES('Studies',  'DisplayedFieldsUpdatedTimestamp', '',                     0, 0, '');

INSERT INTO 'ColumnDisplayProperties' VALUES('Series',   'SeriesInstanceUID',               '',                     0, 0, '');
INSERT INTO 'ColumnDisplayProperties' VALUES('Series',   'StudyInstanceUID',                '',                     0, 0, '');
INSERT INTO 'ColumnDisplayProperties' VALUES('Series',   'SeriesNumber',                    'Series #',             1, 0, '{"resizeMode":"resizeToContents", "sort": "ascending"}');
INSERT INTO 'ColumnDisplayProperties' VALUES('Series',   'SeriesDate',                      'Series date',          0, 0, '');
INSERT INTO 'ColumnDisplayProperties' VALUES('Series',   'SeriesTime',                      'Series time',          1, 3, '');
INSERT INTO 'ColumnDisplayProperties' VALUES('Series',   'SeriesDescription',               'Series description',   1, 2, '{"resizeMode":"stretch"}');
INSERT INTO 'ColumnDisplayProperties' VALUES('Series',   'Modality',                        'Modality',             1, 1, '{"resizeMode":"resizeToContents"}');
INSERT INTO 'ColumnDisplayProperties' VALUES('Series',   'BodyPartExamined',                'Body part',            0, 0, '');
INSERT INTO 'ColumnDisplayProperties' VALUES('Series',   'FrameOfReferenceUID',             '',                     0, 0, '');
INSERT INTO 'ColumnDisplayProperties' VALUES('Series',   'AcquisitionNumber',               'Acquisition #',        0, 0, '');
INSERT INTO 'ColumnDisplayProperties' VALUES('Series',   'ContrastAgent',                   'Contrast agent',       0, 0, '');
INSERT INTO 'ColumnDisplayProperties' VALUES('Series',   'ScanningSequence',                'Scanning sequence',    0, 0, '');
INSERT INTO 'ColumnDisplayProperties' VALUES('Series',   'EchoNumber',                      'Echo #',               0, 0, '');
INSERT INTO 'ColumnDisplayProperties' VALUES('Series',   'TemporalPosition',                'Temporal position',    0, 0, '');
INSERT INTO 'ColumnDisplayProperties' VALUES('Series',   'InsertTimestamp',                 'Date added',           0, 6, '');
INSERT INTO 'ColumnDisplayProperties' VALUES('Series',   'DisplayedSize',                   'Size',                 0, 4, '');
INSERT INTO 'ColumnDisplayProperties' VALUES('Series',   'DisplayedCount',                  'Count',                0, 5, '');
INSERT INTO 'ColumnDisplayProperties' VALUES('Series',   'DisplayedNumberOfFrames',         'Number of frames',     0, 0, '');
INSERT INTO 'ColumnDisplayProperties' VALUES('Series',   'DisplayedFieldsUpdatedTimestamp', '',                     0, 0, '');
