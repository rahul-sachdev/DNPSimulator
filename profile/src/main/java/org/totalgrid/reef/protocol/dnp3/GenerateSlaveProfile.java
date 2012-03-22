package org.totalgrid.reef.protocol.dnp3;


import org.totalgrid.reef.protocol.dnp3.profile.*;

import javax.xml.bind.JAXBContext;
import javax.xml.bind.JAXBException;
import javax.xml.bind.Marshaller;
import javax.xml.datatype.DatatypeConfigurationException;
import javax.xml.datatype.DatatypeFactory;
import javax.xml.datatype.XMLGregorianCalendar;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.math.BigInteger;


class GenerateSlaveProfile {

    public static void main(String[] args)  throws DatatypeConfigurationException, JAXBException, FileNotFoundException {
        DNP3DeviceProfileDocument doc = new DNP3DeviceProfileDocument();
        addHeader(doc);
        addReferenceDevice(doc);

        //output the document
        JAXBContext context = JAXBContext.newInstance("org.totalgrid.reef.protocol.dnp3.profile");
        Marshaller marshaller = context.createMarshaller();
        marshaller.setProperty(Marshaller.JAXB_FORMATTED_OUTPUT, new Boolean(true));
        marshaller.marshal(doc, new FileOutputStream("document.xml"));
    }

    static void addHeader(DNP3DeviceProfileDocument doc) throws DatatypeConfigurationException
    {
        DocumentHeaderType header = new DocumentHeaderType();
        header.setDocumentName("Open DNP3 Slave Profile");
        header.setDocumentDescription("DNP3 slave profile for the Open DNP3 protocol stack");

        addRevisionHistory(header, "J Adam Crain", "2012-03-12", "Initial defintion");
        doc.setDocumentHeader(header);
    }

    static void addReferenceDevice(DNP3DeviceProfileDocument doc)
    {
        Dnp3DeviceOptionalType device = new Dnp3DeviceOptionalType();
        addConfiguration(device);
        doc.setReferenceDevice(device);

    }

    static void addConfiguration(Dnp3DeviceOptionalType device)
    {

    }

    static void addDeviceFunction(DeviceConfigType config)
    {
        DeviceFunctionType function = new DeviceFunctionType();

        DeviceFunctionCapabilitiesType capabilites = new DeviceFunctionCapabilitiesType();
        capabilites.setOutstation(new EmptyElement());
        function.setCapabilities(capabilites);
        DeviceFunctionCurrentValueType currentValue = new DeviceFunctionCurrentValueType();
        function.setCurrentValue(currentValue);

        config.setDeviceFunction(function);
    }

    static void addRevisionHistory(DocumentHeaderType header, String author, String date, String reason) throws DatatypeConfigurationException
    {
       DocumentHeaderType.RevisionHistory rh = new DocumentHeaderType.RevisionHistory();
       rh.setAuthor(author);
       XMLGregorianCalendar gc = DatatypeFactory.newInstance().newXMLGregorianCalendar(date);
       rh.setDate(gc);
       rh.setVersion(new BigInteger(Integer.toString(header.getRevisionHistory().size() + 1)));
       rh.setReason(reason);
       header.getRevisionHistory().add(rh);
    }

}
