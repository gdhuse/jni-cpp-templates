/*-----------------------------------------------------------------------------
 * Example: a Java object of type 'JniExample' is passed to the
 *          native method, which changes the original values of
 *          the member variables.
 *---------------------------------------------------------------------------*/
 
import java.io.*;

public class JniExample {
   public int intField = 17;		   					// integer field
   public static String stringField = "Hello, world!";  // static String field
   public int[] intArray = new int[2];			   		// integer array

   // load the library which implements the native method
   static {
	  try {
		 System.loadLibrary("jni_example");
	  }
	  catch (Throwable e) {
		 System.out.println("Exception: " + e);
	  }
   }
   // sample native call using the proposed framework
   private static native void native_call(JniExample x);
   // sample native call without the proposed JNI encapsulation
   private static native void org_native_call(JniExample x);
   
   public static void main(String[] args) {
	  // initialization
	  JniExample x = new JniExample();
	  x.intArray[0] = 19;
	  x.intArray[1] = 23;

	  // print old values
	  System.out.println("Before native call:");
	  System.out.println("  intField = " + x.intField +
						 ", stringField = " + x.stringField);
	  System.out.println("  intArray[0] = " + x.intArray[0] +
						 ", intArray[1] = " + x.intArray[1]);
	  try {
		 // invoke the native method
		 if (args.length == 0)
			native_call(x);		// call the method with JNI encapsulation
		 else
			org_native_call(x);	// call the method without JNI encapsulation
	  }
	  catch (Throwable e) {
		 System.out.println("Exception: " + e);
	  }

	  // print new values
	  System.out.println("After native call:");
	  System.out.println("  intField = " + x.intField +
						 ", stringField = " + x.stringField);
	  System.out.println("  intArray[0] = " + x.intArray[0] +
						 ", intArray[1] = " + x.intArray[1]);
   }
}
   
