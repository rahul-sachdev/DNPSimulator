using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using DNP3.Interface;
using TestSetControlLibrary;
using System.ComponentModel;

namespace DotNetTestSet
{
    class SynchronizingMeasurementSource : IMeasurementSource
    {
        ISynchronizeInvoke sync;

        public SynchronizingMeasurementSource(ISynchronizeInvoke sync)
        {
            this.sync = sync;
        }


        public void Update(Binary meas, UInt32 index, String id)
        {
            if (this.BinaryUpdate != null) sync.BeginInvoke(new Action(() => BinaryUpdate(meas, index, id)), null);
        }

        public void Update(Analog meas, UInt32 index, String id)
        {
            if (this.AnalogUpdate != null) sync.BeginInvoke(new Action(() => AnalogUpdate(meas, index, id)), null);
        }

        public void Update(Counter meas, UInt32 index, String id)
        {
            if (this.CounterUpdate != null) sync.BeginInvoke(new Action(() => CounterUpdate(meas, index, id)), null);
        }

        public void Update(ControlStatus meas, UInt32 index, String id)
        {
            if (this.ControlStatusUpdate != null) sync.BeginInvoke(new Action(() => ControlStatusUpdate(meas, index, id)), null);
        }

        public void Update(SetpointStatus meas, UInt32 index, String id)
        {
            if (this.SetpointStatusUpdate != null) sync.BeginInvoke(new Action(() => SetpointStatusUpdate(meas, index, id)), null);
        }


        #region IMeasurementSource Members

        public event OnUpdateBinary BinaryUpdate;

        public event OnUpdateAnalog AnalogUpdate;

        public event OnUpdateCounter CounterUpdate;

        public event OnUpdateControlStatus ControlStatusUpdate;

        public event OnUpdateSetpointStatus SetpointStatusUpdate;

        #endregion
    }

    class EventedDataObserver : IDataObserver
    {
        private readonly SynchronizingMeasurementSource source;

        public EventedDataObserver(ISynchronizeInvoke sync)
        {
            source = new SynchronizingMeasurementSource(sync);
        }

        public IMeasurementSource MeasurementSource
        {
            get 
            {
                return source;
            }
        }

        public void Start()
        {
            
        }

        public void Update(Binary update, uint index)
        {
            source.Update(update, index, "");
        }

        public void Update(Analog update, uint index)
        {
            source.Update(update, index, "");
        }

        public void Update(Counter update, uint index)
        {
            source.Update(update, index, "");
        }

        public void Update(ControlStatus update, uint index)
        {
            source.Update(update, index, "");
        }

        public void Update(SetpointStatus update, uint index)
        {
            source.Update(update, index, "");
        }

        public void End()
        {

        }
    }
}
