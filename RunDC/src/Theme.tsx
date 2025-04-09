import { ThemeProvider, createTheme } from "@mui/material/styles";
import CssBaseline from "@mui/material/CssBaseline";
import { ReactNode } from "react";

const darkTheme = createTheme({
    // palette: {
    //     mode: "dark",
    // },
    colorSchemes: {
        dark: true,
    },
});

export type ThemeProps = {
    children: ReactNode;
};

const Theme = (props: ThemeProps) => {
    const { children } = props;

    return (
        <ThemeProvider theme={darkTheme}>
            <CssBaseline />
            {children}
        </ThemeProvider>
    );
};

export default Theme;
