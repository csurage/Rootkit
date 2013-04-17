#include <stdio.h>
#include "windows.h"
#include "winsvc.h"


/*
Get a handle to the SCM database and then register the service
*/
SC_HANDLE installdriver(LPCTSTR drivername, LPCTSTR binarypath)
{
	SC_HANDLE scmdbhandle = NULL;
	SC_HANDLE svchandle = NULL;

	//open a handle to the SCM
	scmdbhandle = OpenSCManager
		(
			NULL,	//MAchine name, NULL = local machine
			NULL,	//database name, NULL = SERVICES_ACTIVE_DATABASE
			SC_MANAGER_ALL_ACCESS	//desired access
		);

	//Check if were able to open a handle to the SCM
	if(scmdbhandle == NULL)
	{
		cout << "installdriver, could not open handle to SCM manager" << endl;
		return NULL;
	}

	//create a service which the SCM will run
	svchandle = CreateService
		(
			scmdbhandle,	//handle to the SC manager
			drivername,		//service name
			drivername,		//display name
			SERVICE_ALL_ACCESS,	//desired access
			SERVICE_KERNEL_DRIVER,	//service type
			SERVICE_DEMAND_START,	//start type, service will start when we call start service
			SERVICE_ERROR_NORMAL,	//error control
			binarypath,		//binary path name
			NULL,		//load order group
			NULL,		//tag id
			NULL,		//Dependancies
			NULL,		//service start name (account name)
			NULL		//password for the account
		);

	//check if we were able to create a new service
	if(svchandle == NULL)
	{
		//check if the service already exists
		if(GetLastError() == ERROR_SERVICE_EXISTS)
		{
			cout << "Error service exists" <<endl;
			svchandle = OpenService(scmdbhandle, drivername, SERVICE_ALL_ACCESS);

			//check if we were able to open the service
			if(svchandle == NULL)
			{
				cout << "Error, could not open handle to the driver" << endl;
				//close the handle to the service
				CloseServiceHandle(scmdbhandle);
				return NULL;
			}
			//close the handle to the service
			CloseServiceHandle(scmdbhandle);
			return NULL;
		}

		cout << "Error, could not open handle to the driver, and the service doesn't exist" << endl;
		//close the handle to the service
		CloseServiceHandle(scmdbhandle);
		return NULL;
	}

	cout << "Success, the service was created" << endl;
	//close the handle to the service
	CloseServiceHandle(scmdbhandle);
	return svchandle;
}

/*
	This function will start running the 
	newly created service we just made
*/
bool loaddriver(SC_HANDLE svchandle)
{
	//load the driver into the kernel, check if we were able
	if(StartService(svchandle,0,NULL) == 0)
	{
		//check if the service is already running
		if(GetLastError() == ERROR_SERVICE_ALREADY_RUNNING)
		{
			cout << "Error, the driver is already running" << endl;
			return true;
		}

		//the driver couldn't be loaded into the kernel
		else
		{
			cout << "Error, the driver could not be loaded into the kernel" <<endl;
			return false;
		}
	}

	cout << "driver successfully loaded"<<endl;
}

/*
	This function will stop the driver from running
*/
bool stopdriver(SC_HANDLE svchandle)
{
	SERVICE_STATUS status;

	//call the function to stop the driver from running
	if(ControlService(svchandle, SERVICE_CONTROL_STOP, &status) == 0)
	{
		cout << "stop driver, failed to unload driver" << endl;
		return false;
	}

	cout << "stop driver, driver successfully unloaded"<<endl;
	return true;
}


/*
	This function will remove the driver from memory
*/
bool deletedriver(SC_HANDLE svchandle)
{
	//call the function to remove the driver from memory
	if(DeleteService(svchandle) == 0)
	{
		cout << "delete driver, failed to delete driver" << endl;
		return false;
	}

	cout << "delete driver, driver successfully deleted"<<endl;
	return true;
}


int main(int argc, char *argv[])
{
}