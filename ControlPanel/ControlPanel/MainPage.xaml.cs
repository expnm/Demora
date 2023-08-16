using Xamarin.Essentials;
using Xamarin.Forms;

namespace ControlPanel
{
    public partial class MainPage : ContentPage
    {
        public MainPage()
        {
            InitializeComponent();
            InitializeButtons();
        }

        void InitializeButtons()
        {
            var flexLayout = new FlexLayout()
            {
                Wrap = FlexWrap.Wrap,
                AlignContent = FlexAlignContent.Center,
                JustifyContent = FlexJustify.Center
            };

            const float buttonSizeScale = 0.1f;
            double buttonWidth = DeviceDisplay.MainDisplayInfo.Width * buttonSizeScale;
            double buttonHeight = DeviceDisplay.MainDisplayInfo.Height * buttonSizeScale;

            foreach (var eventButton in Types.EventButtons)
            {
                var newButton = new Button()
                {
                    Text = eventButton.Caption,
                    WidthRequest = buttonWidth,
                    HeightRequest = buttonHeight,
                    HorizontalOptions = LayoutOptions.Center,
                    VerticalOptions = LayoutOptions.CenterAndExpand,
                    CommandParameter = eventButton.Event,
                };

                newButton.Clicked += (s, e) => { 
                    var clickedButton = s as Button;
                    var controlPanelEvent = clickedButton.CommandParameter as Types.EControlPanelEvent?;
                    if (controlPanelEvent.HasValue)
                    {
                        string message = ((int)controlPanelEvent).ToString();
                        Helper.SendMessage(message, ipControl.Text, int.Parse(portControl.Text));
                    }
                };

                flexLayout.Children.Add(newButton);
            }

            stackLayout.Children.Add(flexLayout);
        }
    }
}
