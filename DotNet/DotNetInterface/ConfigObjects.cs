using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace DNP3.Interface
{
    public class LinkConfig
	{
		
		public LinkConfig(	bool isMaster, 
						    bool useConfirms, 
						    System.UInt32 numRetry, 
						    System.UInt16 localAddr, 
						    System.UInt16 remoteAddr, 
						    System.UInt64 timeout)							
		{
            this.isMaster = isMaster;
            this.useConfirms = useConfirms;
            this.numRetry = numRetry;
            this.localAddr = localAddr;
            this.remoteAddr = remoteAddr;
            this.timeout = timeout;
        }


		public LinkConfig(bool isMaster, bool useConfirms)
        {
			this.isMaster = isMaster;
			this.useConfirms = useConfirms;
			this.numRetry = 0;
			this.localAddr = (ushort) (isMaster ? 1 : 1024);
			this.remoteAddr = (ushort) (isMaster ? 1024 : 1);
			this.timeout = 1000;
		}


		/// <summary>
		/// The master/slave bit set on all messages
		/// </summary>
		public bool isMaster;

		/// <summary>
		/// If true, the link layer will send data requesting confirmation
		/// </summary> 
		public bool useConfirms;

		/// <summary>
		/// The number of retry attempts the link will attempt after the initial try
		/// </summary>
		public System.UInt32 numRetry;

		/// <summary>
		/// dnp3 address of the local device
		/// </summary>
		public System.UInt16 localAddr;

		/// <summary>
		/// dnp3 address of the remote device
		/// </summary>
		public System.UInt16 remoteAddr;

		/// <summary>
		/// the response timeout in milliseconds for confirmed requests
		/// </summary>
		public System.UInt64 timeout;
	}

	public class AppConfig
	{		
		public AppConfig()
        {
            this.rspTimeout = 5000;
            this.numRetry = 0;
            this.fragSize = 2048;
        }

		public AppConfig(System.Int64 rspTimeout, System.Int32 numRetry, System.Int32 fragSize)
        {
			this.rspTimeout = rspTimeout;
			this.numRetry = numRetry;
			this.fragSize = fragSize;
        }
		

		/// <summary>
		/// The response/confirm timeout in millisec
		/// </summary>
		public System.Int64 rspTimeout;

		/// <summary>
		/// Number of retries performed for applicable frames
		/// </summary>
        public System.Int32 numRetry;

		/// <summary>
		/// The maximum size of received application layer fragments
		/// </summary>
        public System.Int32 fragSize;
	}
	
	public class ExceptionScan {
		
        public ExceptionScan(System.Int32 classMask, System.Int64 scanRateMs)
        {
		    this.classMask = classMask;
            this.scanRateMs = scanRateMs;
	    }

        public System.Int32 classMask;
        public System.Int64 scanRateMs;

	};
	
	public enum PointClass {
		PC_CLASS_0 = 0x01,
		PC_CLASS_1 = 0x02,
		PC_CLASS_2 = 0x04,
		PC_CLASS_3 = 0x08,
		PC_ALL_EVENTS = PC_CLASS_1 | PC_CLASS_2 | PC_CLASS_3		
	}

	/// Configuration information for the dnp3 master
	public class MasterConfig {
				
		public MasterConfig()
        {
			fragSize = 2048;
			allowTimeSync = true;
			doUnsolOnStartup = false;
			enableUnsol = true;
			unsolClassMask = (System.Int32) (PointClass.PC_ALL_EVENTS);
			integrityRate = 5000;
			taskRetryRate = 5000;
			scans = new List<ExceptionScan>();
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="classMask"></param>
		/// <param name="period"></param>
		public void AddExceptionScan(System.Int32 classMask, System.Int64 period)
        {
			scans.Add(new ExceptionScan(classMask, period));			
		}

		/// <summary>
        /// Maximum fragment size to use for requests
		/// </summary>
		public System.Int32 fragSize;	

		/// <summary>
        /// If true, the master will do time syncs when it sees the time IIN bit from the slave
		/// </summary>
		public System.Boolean allowTimeSync;

		/// <summary>
        /// If true, the master will enable/disable unsol on startup
		/// </summary>
		public System.Boolean doUnsolOnStartup;

		/// <summary>
        /// If DoUnsolOnStartup == true, the master will use this bit to decide wether to enable (true) or disable (false)
		/// </summary>
		public System.Boolean enableUnsol;

		/// <summary>
        /// Bitwise mask used determine which classes are enabled/disabled for unsol
		/// </summary>
		public System.Int32 unsolClassMask;

		/// <summary>
        /// Period for integrity scans (class 0), -1 for non periodic
		/// </summary>
		public System.Int64 integrityRate;

		/// <summary>
        /// Time delay between task retries
		/// </summary>
		public System.Int64 taskRetryRate;

		/// <summary>
        /// vector that holds exception scans
		/// </summary>
		public readonly List<ExceptionScan> scans;
	};

	public class MasterStackConfig
    {	
		public MasterStackConfig()
        {
            this.link = new LinkConfig(true, false);
            this.master = new MasterConfig();
            this.app = new AppConfig();
		}

        public readonly MasterConfig master;
        public readonly AppConfig app;
        public readonly LinkConfig link;
	};
}
