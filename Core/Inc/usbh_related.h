
#include "usb_host.h"
#include "usbh_core.h"

// Only declare it as external – do NOT define it
extern USBH_HandleTypeDef hUsbHostFS;
uint8_t  midi_rx_buf[64];
uint8_t  midi_pipe_in  = 0;
uint8_t  midi_ready    = 0;
uint8_t  midi_tx_buf[64];
uint8_t  midi_pipe_out = 0;
extern uint8_t cdc_buffer[64];




void DumpUSBDeviceInfo(USBH_HandleTypeDef *phost)
 	{
 	  printf("\r\n========== USB DEVICE INFO ==========\r\n");
 	  printf("VID: 0x%04X   PID: 0x%04X\r\n",
 	         phost->device.DevDesc.idVendor,
 	         phost->device.DevDesc.idProduct);

 	  printf("Number of Configurations : %d\r\n", phost->device.DevDesc.bNumConfigurations);
 	  printf("Number of Interfaces     : %d\r\n", phost->device.CfgDesc.bNumInterfaces);

 	  for(uint8_t i = 0; i < phost->device.CfgDesc.bNumInterfaces; i++)
 	  {
 	    USBH_InterfaceDescTypeDef *itf = &phost->device.CfgDesc.Itf_Desc[i];

 	    printf("\r\n--- Interface %d ---\r\n", i);
 	    printf("  bInterfaceNumber   : %d\r\n", itf->bInterfaceNumber);
 	    printf("  bAlternateSetting  : %d\r\n", itf->bAlternateSetting);
 	    printf("  bNumEndpoints      : %d\r\n", itf->bNumEndpoints);
 	    printf("  bInterfaceClass    : 0x%02X\r\n", itf->bInterfaceClass);
 	    printf("  bInterfaceSubClass : 0x%02X\r\n", itf->bInterfaceSubClass);
 	    printf("  bInterfaceProtocol : 0x%02X\r\n", itf->bInterfaceProtocol);

 	    for(uint8_t ep = 0; ep < itf->bNumEndpoints; ep++)
 	    {
 	      USBH_EpDescTypeDef *endpoint = &itf->Ep_Desc[ep];

 	      printf("    Endpoint %d:\r\n", ep);
 	      printf("      bEndpointAddress : 0x%02X  (%s)\r\n",
 	             endpoint->bEndpointAddress,
 	             (endpoint->bEndpointAddress & 0x80) ? "IN" : "OUT");
 	      printf("      bmAttributes     : 0x%02X  ", endpoint->bmAttributes);

 	      switch(endpoint->bmAttributes & 0x03)
 	      {
 	        case 0: printf("(Control)\r\n"); break;
 	        case 1: printf("(Isochronous)\r\n"); break;
 	        case 2: printf("(Bulk)\r\n"); break;
 	        case 3: printf("(Interrupt)\r\n"); break;
 	      }

 	      printf("      wMaxPacketSize   : %d\r\n", endpoint->wMaxPacketSize);
 	      printf("      bInterval        : %d\r\n", endpoint->bInterval);
 	    }
 	  }
 	  printf("====================================\r\n\r\n");
 	}
void usb_print(void)	{  if(hUsbHostFS.gState == HOST_CHECK_CLASS || hUsbHostFS.gState == HOST_ABORT_STATE)
	  {
	    static uint8_t dumped = 0;
	    if(!dumped)
	    {
	      DumpUSBDeviceInfo(&hUsbHostFS);
	      dumped = 1;
	    }
	  }
}
void midi_rec(void){
	if (midi_ready)
	{
	  USBH_URBStateTypeDef urb = USBH_LL_GetURBState(&hUsbHostFS, midi_pipe_in);

	  if (urb == USBH_URB_DONE)
	  {
	    uint16_t len = USBH_LL_GetLastXferSize(&hUsbHostFS, midi_pipe_in);
	    printf("Received %d bytes\r\n", len);

	    for (uint16_t i = 0; i < len; i += 4)
	    {
	      uint8_t cin  = midi_rx_buf[i] & 0x0F;
	      uint8_t b1   = midi_rx_buf[i+1];
	      uint8_t b2   = midi_rx_buf[i+2];
	      uint8_t b3   = midi_rx_buf[i+3];

	      if (cin == 0x09) printf("Note On  %d %d %d\r\n", b1&0x0F, b2, b3);
	      if (cin == 0x08) printf("Note Off %d %d %d\r\n", b1&0x0F, b2, b3);
	      if (cin == 0x0B) printf("CC       %d %d %d\r\n", b1&0x0F, b2, b3);
	    }

	    // Re-submit immediately
	    USBH_BulkReceiveData(&hUsbHostFS, midi_rx_buf, 64, midi_pipe_in);
	  }
	  else if (urb == USBH_URB_ERROR || urb == USBH_URB_STALL)
	  {
	    printf("URB Error/Stall – reopening\r\n");
	    // optional recovery
	  }
	}
}

void usb_loop_start(void){
	MX_USB_HOST_Process();

	// --- Open the MIDI pipe once the device is fully enumerated ---
	if (hUsbHostFS.device.is_connected &&
	    hUsbHostFS.device.address != 0 &&
	    midi_ready == 0 &&
	    (hUsbHostFS.gState == HOST_CHECK_CLASS ||
	     hUsbHostFS.gState == HOST_ABORT_STATE ||
	     hUsbHostFS.gState == HOST_CLASS))
	{
	  printf("Opening MIDI Bulk IN pipe (EP 0x81)...\r\n");

	  midi_pipe_in = USBH_AllocPipe(&hUsbHostFS, 0x81);

	  if (USBH_OpenPipe(&hUsbHostFS,
	                    midi_pipe_in,
	                    0x81,                          // Bulk IN
	                    hUsbHostFS.device.address,
	                    hUsbHostFS.device.speed,
	                    USB_EP_TYPE_BULK,
	                    64) == USBH_OK)
	  {
	    printf("Pipe opened OK\r\n");
	    midi_ready = 1;
	    // Inside the same "if (midi_ready == 0)" block where you open the IN pipe

	    // --- Bulk OUT pipe (0x01) for sending to the keyboard ---
	    midi_pipe_out = USBH_AllocPipe(&hUsbHostFS, 0x01);

	    if (USBH_OpenPipe(&hUsbHostFS,
	                      midi_pipe_out,
	                      0x01,                          // Bulk OUT
	                      hUsbHostFS.device.address,
	                      hUsbHostFS.device.speed,
	                      USB_EP_TYPE_BULK,
	                      64) == USBH_OK)
	    {
	      printf("OUT pipe opened OK\r\n");
	    }
	    else
	    {
	      printf("Failed to open OUT pipe\r\n");
	    }
	    // Start the first receive
	    USBH_BulkReceiveData(&hUsbHostFS, midi_rx_buf, 64, midi_pipe_in);
	  }
	  else
	  {
	    printf("Failed to open pipe\r\n");
	  }
	}

	// --- Receive & parse ---
	if (midi_ready)
	{
	  USBH_URBStateTypeDef state = USBH_LL_GetURBState(&hUsbHostFS, midi_pipe_in);

	  if (state == USBH_URB_DONE)
	  {
	    uint16_t len = USBH_LL_GetLastXferSize(&hUsbHostFS, midi_pipe_in);
	    printf("Got %d bytes\r\n", len);

	    for (uint16_t i = 0; i < len; i += 4)
	    {
	      uint8_t cin = midi_rx_buf[i] & 0x0F;
	      uint8_t b1  = midi_rx_buf[i+1];
	      uint8_t b2  = midi_rx_buf[i+2];
	      uint8_t b3  = midi_rx_buf[i+3];

	      if (cin == 0x09) printf("Note On  ch:%d note:%d vel:%d\r\n", b1 & 0x0F, b2, b3);
	      if (cin == 0x08) printf("Note Off ch:%d note:%d vel:%d\r\n", b1 & 0x0F, b2, b3);
	      if (cin == 0x0B) printf("CC       ch:%d cc:%d  val:%d\r\n", b1 & 0x0F, b2, b3);
	    }
	    memcpy(cdc_buffer,midi_rx_buf+1,4);
	    // Re-arm the receive
	    USBH_BulkReceiveData(&hUsbHostFS, midi_rx_buf, 64, midi_pipe_in);
	  }
	}


}
void MIDI_Send(uint8_t cin, uint8_t byte1, uint8_t byte2, uint8_t byte3)
{
  if (!midi_ready) return ;

  // Wait until previous TX is finished
  while (USBH_LL_GetURBState(&hUsbHostFS, midi_pipe_out) == USBH_URB_IDLE ||
         USBH_LL_GetURBState(&hUsbHostFS, midi_pipe_out) == USBH_URB_DONE)
  {
    // ready
    break;
  }

  // Build a single 4-byte USB-MIDI packet
  midi_tx_buf[0] = cin;      // Code Index Number + cable 0
  midi_tx_buf[1] = byte1;
  midi_tx_buf[2] = byte2;
  midi_tx_buf[3] = byte3;

  // Optional: zero the rest of the buffer
  for (int i = 4; i < 64; i++) midi_tx_buf[i] = 0;

  if (USBH_BulkSendData(&hUsbHostFS, midi_tx_buf, 4, midi_pipe_out, 0) == USBH_OK)
  {
    return ;
  }
  return ;
}
