namespace Hazel
{
    public class Input
    {
        public static bool IsKeyDown(KeyCode keycode)
        {
            return InternalCalls.Input_IsKeyDown(keycode);
        }

        public static bool IsShiftDown()
        {
            return InternalCalls.Input_IsKeyDown(KeyCode.LeftShift) | InternalCalls.Input_IsKeyDown(KeyCode.RightShift);
        }

        public static bool IsCtrlDown()
        {
            return InternalCalls.Input_IsKeyDown(KeyCode.LeftControl) | InternalCalls.Input_IsKeyDown(KeyCode.RightControl);
        }

        public static bool IsAltDown()
        {
            return InternalCalls.Input_IsKeyDown(KeyCode.LeftAlt) | InternalCalls.Input_IsKeyDown(KeyCode.RightAlt);
        }
    }
}