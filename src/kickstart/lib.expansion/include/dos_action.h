#ifndef dos_action_h
#define dos_action_h

/* Does nothing*/
#define FSA_NIL				0
#define FSA_STARTMESSAGE   0
/* Opens a Filehandle for reading*/
#define FSA_OPEN			 1
/* Closes an opened filehandle. Takes no extra arguments. */
#define FSA_CLOSE			 2
/* Reads from a filehandle into a buffer. */
#define FSA_READ			 3
/* Writes the contents of a buffer into a filehandle. Uses IFS_READ_WRITE. */
#define FSA_WRITE			 4
/* The action does exactly the same as the function Seek(). */
#define FSA_SEEK			 5
/* Sets the size of filehandle. Uses IFS_SEEK (see above) as argument array. */
#define FSA_SET_FILE_SIZE	 6
/* Waits for a character to arrive at the filehandle. This is not used for
   plain files, but for queues only. Optionally a maximum time to wait may be
   specified. */
#define FSA_WAIT_CHAR		 7
/* Applies a new mode to a file. If you supply io_Mask with a value of 0,
   no changes are made and you can just read the resulting io_FileMode. */
#define FSA_FILE_MODE		 8
/* This action can be used to query if a filehandle is interactive, ie if it
   is a terminal or not. */
#define FSA_IS_INTERACTIVE	 9
/* Compares two locks for equality. */
#define FSA_SAME_LOCK		10
/* Examines a filehandle, giving various information about it. */
#define FSA_EXAMINE			11
#define FSA_EXAMINE_NEXT	12
/* Works exactly like FSA_EXAMINE with the exeption that multiple files may be
   examined, ie the filehandle must be a directory. */
#define FSA_EXAMINE_ALL		13
/* This has to be called, if FSA_EXAMINE_ALL is stopped before all examined
   files were returned. It takes no arguments except the filehandle in
   io_Unit. */
#define FSA_EXAMINE_ALL_END 14

#define FSA_INFO 15
/* Creates a new directory. The filehandle of that new directory is returned.*/
#define FSA_CREATE_DIR		16
/* Creates a hard link (ie gives one file a second name). */
#define FSA_CREATE_HARDLINK 17
/* Creates a soft link (ie a file is created, which references another by its
   name). */
#define FSA_CREATE_SOFTLINK 18
/* Renames a file. To the old and the new name, the current directory is
   applied to. */
#define FSA_RENAME			19
/* Resolves the full path name of the file a softlink filehandle points to. */
#define FSA_READ_SOFTLINK	20
/* Deletes an object on the volume. */
#define FSA_DELETE_OBJECT	21
/* Sets a filecomment for a file. */
#define FSA_SET_COMMENT		22
/* Sets the protection bits of a file. */
#define FSA_SET_PROTECT		23
/* Sets the ownership of a file. */
#define FSA_SET_OWNER		24
/* Sets the last modification date of the filename given as first argument.
   The date is given as standard TimeStamp structure (see <dos/dos.h>) as
   second to fourth argument (ie as days, minutes and ticks). */
#define FSA_SET_DATE		25
/* Check if a filesystem is in fact a FILEsystem, ie can contain different
   files. */
#define FSA_IS_FILESYSTEM	26
/* Changes the number of buffers for the filesystem. The current number of
   buffers is returned. The size of the buffers is filesystem-dependent. */
#define FSA_MORE_CACHE		27
/* Formats a volume, ie erases all data on it. */
#define FSA_FORMAT			28
/* Resets/Reads the mount-mode of the volume passed in as io_Unit. The first
   and second argument work exactly like FSA_FILE_MODE, but the third
   argument can contain a password, if MMF_LOCKED is set. */
#define FSA_MOUNT_MODE		29
/* The following actions are currently not supported. */
#define FSA_SERIALIZE_DISK  30
#define FSA_FLUSH			31
#define FSA_INHIBIT			32
#define FSA_WRITE_PROTECT   33
#define FSA_DISK_CHANGE     34
#define FSA_ADD_NOTIFY	    35
/* Uses IFS_NOTIFY */
#define FSA_REMOVE_NOTIFY   36
#define FSA_DISK_INFO	    37
#define FSA_CHANGE_SIGNAL   38
#define FSA_LOCK_RECORD     39
#define FSA_UNLOCK_RECORD   40
#define FSA_PARENT_FH       41
#define FSA_PARENT_DIR      42
/*   Allows us to change a console between raw and cooked mode.*/
#define FSA_CONSOLE_MODE    43
#define FSA_LOCATE_OBJECT	44
#define FSA_FREE_LOCK		45

#endif
