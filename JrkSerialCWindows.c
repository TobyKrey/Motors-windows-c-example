// Uses Windows-specific functions to send and receive data from a jrk.
// NOTE: The jrk's input mode must be "Serial".
// NOTE: The jrk's serial mode must be set to "USB Dual Port".
// NOTE: You might need to change the value of portName below.


#include <stdio.h>
#include <string.h>
#include <windows.h>

int Set_target(); 
/** Opens a handle to a serial port in Windows using CreateFile.
 * portName: The name of the port.
 *   Examples: "COM4", "\\\\.\\USBSER000", "USB#VID_1FFB&PID_0089&MI_04#6&3ad40bf600004#
 * baudRate: The baud rate in bits per second.
 * Returns INVALID_HANDLE_VALUE if it fails.  Otherwise returns a handle to the port. */
HANDLE openPort(const char * portName, unsigned int baudRate)
{
	HANDLE port;
	DCB commState;
	BOOL success;
	COMMTIMEOUTS timeouts;
	/* Open the serial port. */
	port = CreateFileA(portName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (port == INVALID_HANDLE_VALUE)
	{
		switch(GetLastError())
		{
		case ERROR_ACCESS_DENIED:	
			fprintf(stderr, "Error: Access denied.  Try closing all other programs that are using the device.\n");
			break;
		case ERROR_FILE_NOT_FOUND:
			fprintf(stderr, "Error: Serial port not found.  "
				"Make sure that \"%s\" is the right port name.  "
				"Try closing all programs using the device and unplugging the "
				"device, or try rebooting.\n", portName);
			break;
		default:
			fprintf(stderr, "Error: Unable to open serial port.  Error code 0x%x.\n", GetLastError());
			break;
		}
		return INVALID_HANDLE_VALUE;
	}

	/* Set the timeouts. */
	success = GetCommTimeouts(port, &timeouts);
	if (!success)
	{
		fprintf(stderr, "Error: Unable to get comm timeouts.  Error code 0x%x.\n", GetLastError());
		CloseHandle(port);
		return INVALID_HANDLE_VALUE;
	}
	timeouts.ReadIntervalTimeout = 1000;
	timeouts.ReadTotalTimeoutConstant = 1000;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = 1000;
	timeouts.WriteTotalTimeoutMultiplier = 0;
	success = SetCommTimeouts(port, &timeouts);
	

	if (!success)
	{
		fprintf(stderr, "Error: Unable to set comm timeouts.  Error code 0x%x.\n", GetLastError());
		CloseHandle(port);
		return INVALID_HANDLE_VALUE;
	}

	/* Set the baud rate. */
	success = GetCommState(port, &commState);
	if (!success)
	{
		fprintf(stderr, "Error: Unable to get comm state.  Error code 0x%x.\n", GetLastError());
		CloseHandle(port);
		return INVALID_HANDLE_VALUE;
	}
	commState.BaudRate = baudRate;
	success = SetCommState(port, &commState);
	if (!success)
	{
		fprintf(stderr, "Error: Unable to set comm state.  Error code 0x%x.\n", GetLastError());
		CloseHandle(port);
		return INVALID_HANDLE_VALUE;
	}

	/* Flush out any bytes received from the device earlier. */
	success = FlushFileBuffers(port);
	if (!success)
	{
		fprintf(stderr, "Error: Unable to flush port buffers.  Error code 0x%x.\n", GetLastError());
		CloseHandle(port);
		return INVALID_HANDLE_VALUE;
	}

	return port;
}

// Gets a two-byte, unsigned variable from the jrk.
int jrkGetVariable(HANDLE port, unsigned char command)
{
	unsigned char response[2];
	BOOL success;
	DWORD bytesTransferred;

	// Send the command to the device.
	success = WriteFile(port, &command, 1, &bytesTransferred, NULL);
	if (!success)
	{
		fprintf(stderr, "Error: Unable to write Get Position command to serial port.  Error code 0x%x.", GetLastError());
		return -1;
	}
	if (sizeof(command) != bytesTransferred)
	{
		fprintf(stderr, "Error: Expected to write %d bytes but only wrote %d.", sizeof(command), bytesTransferred);
		return -1;
	}

	// Read the response from the device.
	success = ReadFile(port, response, sizeof(response), &bytesTransferred, NULL);
	if (!success)
	{
		fprintf(stderr, "Error: Unable to read Get Position response from serial port.  Error code 0x%x.", GetLastError());
		return 0;
	}
	if (sizeof(response) != bytesTransferred)
	{
		fprintf(stderr, "Error: Expected to read %d bytes but only read %d (timeout). "
			"Make sure the jrk's serial mode is USB Dual Port or USB Chained.", sizeof(command), bytesTransferred);
		return 0;
	}

	return response[0] + 256*response[1];
}

// Gets the value of the jrk's Feedback variable (0-4095).
int jrkGetFeedback(HANDLE port)
{
	return jrkGetVariable(port, 0xA5);
}

// Gets the value of the jrk's Target variable (0-4095).
int jrkGetTarget(HANDLE port)
{
	return jrkGetVariable(port, 0xA3);
}

// Sets the jrk's Target variable (0-4095).
BOOL jrkSetTarget(HANDLE port, unsigned short target)
{
	unsigned char command[2];
	DWORD bytesTransferred;
	BOOL success;

	// Compose the command.
	command[0] = 0xC0 + (target & 0x1F);
	command[1] = (target >> 5) & 0x7F;

	// Send the command to the device.
	success = WriteFile(port, command, sizeof(command), &bytesTransferred, NULL);
	if (!success)
	{
		fprintf(stderr, "Error: Unable to write Set Target command to serial port.  Error code 0x%x.", GetLastError());
		return 0;
	}
	if (sizeof(command) != bytesTransferred)
	{
		fprintf(stderr, "Error: Expected to write %d bytes but only wrote %d.", sizeof(command), bytesTransferred);
		return 0;
	}

	return 1;
}

/** This is the first function to run when the program starts. */
int main(int argc, char * argv[])
{
	HANDLE port;
	char * portName;
	int baudRate;
	int feedback, target, newTarget;
	int i,user_in =2 ;

	while(1){
	user_in=2;
	printf("Do you wish to Run the motor?(1/0) \n"); 
		while( user_in != 1 && user_in != 0)
		{
			scanf("%d",&user_in);
		}
	
		if(user_in == 1)
		{

										/* portName should be the name of the jrk's Command Port (e.g. "\\\\.\\COM4")
	* as shown in your computer's Device Manager.
	* Alternatively you can use \\.\USBSER000 to specify the first virtual COM
	* port that uses the usbser.sys driver.  This will usually be the jrk's
	* command port. */
			portName = "\\\\.\\COM5";  // Each double slash in this source code represents one slash in the actual name.

			/* Choose the baud rate (bits per second).
				* If the jrk's serial mode is USB Dual Port, this number does not matter. */
			baudRate = 9600;

			/* Open the jrk's serial port. */
			port = openPort(portName, baudRate);
			if (port == INVALID_HANDLE_VALUE){ return -1; }

			feedback = jrkGetFeedback(port);
			printf("Current Feedback is %d.\n", feedback); 

			target = jrkGetTarget(port);
			printf("Current Target is %d.\n", target);

			newTarget  = Set_target();
			//newTarget = (target < 2048) ? 3000 : 1000;
			printf("Setting Target to %d.\n", newTarget);
			jrkSetTarget(port, newTarget);

			CloseHandle(port);


			
		}
	else
	{
	return 0;
	}
	}
}
// Set up for new target from user
// yet to be calibrated 
int Set_target()
{ 
	int degrees =0;
	int target  =0;
	int motor_max=4095;
	int motor_min=0;
	do{
		printf("Insert a target value from -45 to 45 degrees  \n");
		scanf("%d",&degrees);
		target=(degrees+125)*4095/270; // degress+ calibration value to move 0 to 0 ruder position
		// 4095/180 max feed back is 4095 which should be at 180 degrees location may need calibration
	}while(target>3500 || target <500);
	return(target);
}
