#ifndef __I_COMMAND_ACCEPTOR_DN_H_
#define __I_COMMAND_ACCEPTOR_DN_H_

public interface ICommandAcceptorDN
{
	void AcceptCommand(BinaryOutputDN^ binaryOutput, int index, int sequence, IResponseAcceptorDN^ rspAcceptor) = 0;
	void AcceptCommand(SetpointDN^ setpoint, size_t, int sequence, IResponseAcceptorDN^ rspAcceptor) = 0;
};

#endif
